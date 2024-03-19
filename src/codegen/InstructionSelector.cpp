#include "Constant.hpp"
#include "GlobalVariable.hpp"
#include "Instruction.hpp"
#include "InstructionSelector.hpp"
#include "MIBuilder.hpp"
#include "MachineBasicBlock.hpp"
#include "MachineInstr.hpp"
#include "Module.hpp"
#include "Operand.hpp"

#include <iostream>
#include <memory>
#include <utility>

std::shared_ptr<Register> InstructionSelector::get_reg(Value *val) {
    if (dynamic_cast<Constant *>(val) != nullptr) {
        if (dynamic_cast<ConstantInt *>(val) != nullptr) {
            auto tmp_reg = VirtualRegister::create(Register::General);
            int value = dynamic_cast<ConstantInt *>(val)->get_value();
            builder.load_int32(context.machine_bb, value, tmp_reg);
            return tmp_reg;
        } else if (dynamic_cast<ConstantFP *>(val) != nullptr) {
            auto tmp_freg = VirtualRegister::create(Register::Float);
            float value = dynamic_cast<ConstantFP *>(val)->get_value();
            builder.load_float(context.machine_bb, value, tmp_freg);
            return tmp_freg;
        } else
            assert(false);
    }
    if (dynamic_cast<GlobalVariable *>(val) != nullptr) {
        auto tmp_reg = VirtualRegister::create(Register::General);
        builder.append_instr(
            context.machine_bb, MachineInstr::Tag::LA_LOCAL,
            {tmp_reg, std::make_shared<Label>(val->get_name())});
        return tmp_reg;
    }
    if (val_map.find(val) != val_map.end()) {
        return val_map[val];
    } else {
        assert(false && "val not found");
    }
}

void InstructionSelector::set_reg(Value *val, std::shared_ptr<Register> reg) {
    val_map[val] = reg;
}

void InstructionSelector::run() {
    // build module, functions and basic blocks
    IR_module = module->get_IR_module();
    IR_module->set_print_name();
    for (auto &func : IR_module->get_functions()) {
        auto machine_func = std::make_shared<MachineFunction>(&func, module);
        func_map[&func] = machine_func;
        if (func.is_declaration())
            continue;
        module->add_function(machine_func);
        for (auto &bb : func.get_basic_blocks()) {
            auto machine_bb =
                std::make_shared<MachineBasicBlock>(&bb, machine_func);
            machine_func->add_basic_block(machine_bb);
            bb_map[&bb] = machine_bb;
            context.machine_bb = machine_bb;
            for (auto &inst : bb.get_instructions()) {
                context.IR_inst = &inst;
            }
        }
    }

    // build CFG
    for (auto &func : IR_module->get_functions()) {
        if (func.is_declaration())
            continue;

        for (auto &bb : func.get_basic_blocks()) {
            context.machine_bb = bb_map[&bb];
            for (auto &pred : bb.get_pre_basic_blocks()) {
                context.machine_bb->add_pre_basic_block(bb_map[pred]);
            }
            for (auto &succ : bb.get_succ_basic_blocks()) {
                context.machine_bb->add_succ_basic_block(bb_map[succ]);
            }
        }
    }

    // gen phi
    for (auto &func : IR_module->get_functions()) {
        if (func.is_declaration())
            continue;
        for (auto &bb : func.get_basic_blocks()) {
            context.machine_bb = bb_map[&bb];
            for (auto &inst : bb.get_instructions()) {
                context.IR_inst = &inst;
                if (inst.get_instr_type() == Instruction::phi) {
                    if (inst.get_type()->is_integer_type() ||
                        inst.get_type()->is_pointer_type()) {
                        auto phi_reg =
                            VirtualRegister::create(Register::General);
                        set_reg(&inst, phi_reg);
                    } else if (inst.get_type()->is_float_type()) {
                        auto phi_freg =
                            VirtualRegister::create(Register::Float);
                        set_reg(&inst, phi_freg);
                    }
                }
            }
        }
    }

    // select instructions
    for (auto &machine_func : module->get_functions()) {
        context.clear();
        context.machine_func = machine_func;

        gen_store_params();
        set_all_regs();
        for (auto &bb : machine_func->get_basic_blocks()) {
            context.machine_bb = bb;

            for (auto &inst : bb->get_IR_basic_block()->get_instructions()) {
                context.IR_inst = &inst;
                gen_inst();
            }
        }

        // gen prologue and epilogue
        auto prologue_mbb =
            std::make_shared<MachineBasicBlock>(nullptr, machine_func, "");
        auto epilogue_mbb =
            std::make_shared<MachineBasicBlock>(nullptr, machine_func, "_exit");

        machine_func->set_prologue_block(prologue_mbb);
        machine_func->set_epilogue_block(epilogue_mbb);

        auto entry_mbb =
            bb_map[machine_func->get_IR_function()->get_entry_block()];
        machine_func->get_prologue_block()->add_succ_basic_block(entry_mbb);
        entry_mbb->add_pre_basic_block(machine_func->get_prologue_block());

        for(auto &bb: machine_func->get_basic_blocks()){
            if(bb->get_IR_basic_block()->get_instructions().back().get_instr_type() == Instruction::ret){
                bb->add_succ_basic_block(machine_func->get_epilogue_block());
                machine_func->get_epilogue_block()->add_pre_basic_block(bb);
            }
        }

        machine_func->add_basic_block(prologue_mbb);
        machine_func->add_basic_block(epilogue_mbb);

        builder.gen_prologue_epilogue(machine_func);

    }
}

void InstructionSelector::set_all_regs() {
    for (auto &bb : context.machine_func->get_basic_blocks()) {
        for (auto &inst : bb->get_IR_basic_block()->get_instructions()) {
            if (not inst.is_void()) {
                if (inst.get_type()->is_integer_type() ||
                    inst.get_type()->is_pointer_type()) {
                    auto reg = VirtualRegister::create(Register::General);
                    set_reg(&inst, reg);
                } else if (inst.get_type()->is_float_type()) {
                    auto freg = VirtualRegister::create(Register::Float);
                    set_reg(&inst, freg);
                }
            }
        }
    }
}

void InstructionSelector::gen_store_params() {

    auto entry_mbb =
        bb_map[context.machine_func->get_IR_function()->get_entry_block()];
    for (auto &arg : context.machine_func->get_IR_function()->get_args()) {
        auto suffix = (arg.get_type()->get_size() == 8)
                          ? MachineInstr::Suffix::DWORD
                          : MachineInstr::Suffix::WORD;
        if (arg.get_type()->is_integer_type() ||
            arg.get_type()->is_pointer_type()) {
            auto tmp_reg = VirtualRegister::create(Register::General);
            if (context.machine_func->params_schedule_map[&arg].on_stack) {
                builder.load_from_stack(
                    entry_mbb, tmp_reg, PhysicalRegister::fp(),
                    context.machine_func->params_schedule_map[&arg].offset,
                    suffix);
                set_reg(&arg, tmp_reg);
            } else {
                auto tmp_reg = VirtualRegister::create(Register::General);
                builder.append_instr(
                    entry_mbb, MachineInstr::Tag::MOV,
                    {tmp_reg,
                     context.machine_func->params_schedule_map[&arg].reg});
            }
            set_reg(&arg, tmp_reg);
        } else if (arg.get_type()->is_float_type()) {
            auto tmp_freg = VirtualRegister::create(Register::Float);
            if (context.machine_func->params_schedule_map[&arg].on_stack) {
                builder.load_from_stack(
                    entry_mbb, tmp_freg, PhysicalRegister::fp(),
                    context.machine_func->params_schedule_map[&arg].offset, suffix);
                set_reg(&arg, tmp_freg);
            } else {
                auto tmp_freg = VirtualRegister::create(Register::Float);
                builder.append_instr(
                    entry_mbb, MachineInstr::Tag::MOV,
                    {tmp_freg,
                     context.machine_func->params_schedule_map[&arg].reg});
            }
            set_reg(&arg, tmp_freg);
        } else
            assert(false);
    }
}

void InstructionSelector::gen_ret() {
    auto *retInst = static_cast<ReturnInst *>(context.IR_inst);
    if (retInst->get_num_operand() > 0) {
        auto *ret_val = retInst->get_operand(0);
        if (ret_val->get_type()->is_integer_type()) {
            builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                                 {PhysicalRegister::a(0), get_reg(ret_val)});
        } else if (ret_val->get_type()->is_float_type())
            builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                                 {PhysicalRegister::fa(0), get_reg(ret_val)});
        else
            assert(false);
    } else
        builder.append_instr(
            context.machine_bb, MachineInstr::Tag::MOV,
            {PhysicalRegister::a(0), PhysicalRegister::zero()});
    builder.append_instr(
        context.machine_bb, MachineInstr::Tag::B,
        {std::make_shared<Label>(context.machine_func->get_name() + "_exit")});
}

void InstructionSelector::gen_br() {
    auto *branchInst = static_cast<BranchInst *>(context.IR_inst);
    if (branchInst->is_cond_br()) {
        auto *cond = branchInst->get_operand(0);
        auto *truebb = static_cast<BasicBlock *>(branchInst->get_operand(1));
        auto *falsebb = static_cast<BasicBlock *>(branchInst->get_operand(2));

        auto tmp_reg = VirtualRegister::create(Register::General);
        builder.append_instr(
            context.machine_bb, MachineInstr::Tag::BNEZ,
            {get_reg(cond), std::make_shared<Label>(bb_map[truebb])});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::B,
                             {std::make_shared<Label>(bb_map[falsebb])});
    } else {
        auto *branchbb = static_cast<BasicBlock *>(branchInst->get_operand(0));
        builder.append_instr(context.machine_bb, MachineInstr::Tag::B,
                             {std::make_shared<Label>(bb_map[branchbb])});
    }
}

void InstructionSelector::gen_binary() {
    auto op1 = get_reg(context.IR_inst->get_operand(0));
    auto op2 = get_reg(context.IR_inst->get_operand(1));
    // 根据指令类型生成汇编
    std::shared_ptr<MachineInstr> inst;
    auto dst_reg = get_reg(context.IR_inst);
    switch (context.IR_inst->get_instr_type()) {
    case Instruction::add:
        inst = builder.append_instr(context.machine_bb, MachineInstr::Tag::ADD,
                                    {dst_reg, op1, op2},
                                    MachineInstr::Suffix::WORD);
        break;
    case Instruction::sub:
        inst = builder.append_instr(context.machine_bb, MachineInstr::Tag::SUB,
                                    {dst_reg, op1, op2},
                                    MachineInstr::Suffix::WORD);
        break;
    case Instruction::mul:
        inst = builder.append_instr(context.machine_bb, MachineInstr::Tag::MUL,
                                    {dst_reg, op1, op2},
                                    MachineInstr::Suffix::WORD);
        break;
    case Instruction::sdiv:
        inst = builder.append_instr(context.machine_bb, MachineInstr::Tag::DIV,
                                    {dst_reg, op1, op2},
                                    MachineInstr::Suffix::WORD);
        break;
    case Instruction::srem:
        inst = builder.append_instr(context.machine_bb, MachineInstr::Tag::MOD,
                                    {dst_reg, op1, op2},
                                    MachineInstr::Suffix::WORD);
        break;
    default:
        assert(false);
    }
    // 将结果填入栈帧中
}

void InstructionSelector::gen_float_binary() {
    auto op1 = get_reg(context.IR_inst->get_operand(0));
    auto op2 = get_reg(context.IR_inst->get_operand(1));
    // 根据指令类型生成汇编
    std::shared_ptr<MachineInstr> inst;
    auto dst_freg = get_reg(context.IR_inst);
    switch (context.IR_inst->get_instr_type()) {
    case Instruction::fadd:
        inst =
            builder.append_instr(context.machine_bb, MachineInstr::Tag::FADD_S,
                                 {dst_freg, op1, op2});
        break;
    case Instruction::fsub:
        inst =
            builder.append_instr(context.machine_bb, MachineInstr::Tag::FSUB_S,
                                 {dst_freg, op1, op2});
        break;
    case Instruction::fmul:
        inst =
            builder.append_instr(context.machine_bb, MachineInstr::Tag::FMUL_S,
                                 {dst_freg, op1, op2});
        break;
    case Instruction::fdiv:
        inst =
            builder.append_instr(context.machine_bb, MachineInstr::Tag::FDIV_S,
                                 {dst_freg, op1, op2});
        break;
    default:
        assert(false);
    }
}

void InstructionSelector::gen_alloca() {
    auto *allocaInst = static_cast<AllocaInst *>(context.IR_inst);
    context.machine_func->frame_scheduler->insert_alloca(allocaInst);
    auto dst_reg = get_reg(allocaInst);

    builder.add_int_to_reg(
        context.machine_bb, dst_reg, PhysicalRegister::fp(),
        -context.machine_func->frame_scheduler->get_alloca_offset(allocaInst));
}

void InstructionSelector::gen_load() {
    auto *ptr = context.IR_inst->get_operand(0);
    auto *type = context.IR_inst->get_type();
    if (type->is_float_type()) {
        auto dst_freg = get_reg(context.IR_inst);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::FLD_S,
                             {dst_freg, get_reg(ptr), Immediate::create(0)});
    } else {
        auto dst_reg = get_reg(context.IR_inst);

        // load 整数类型的数据
        if (type->get_size() == 1)
            builder.append_instr(context.machine_bb, MachineInstr::Tag::LD,
                                 {dst_reg, get_reg(ptr), Immediate::create(0)},
                                 MachineInstr::Suffix::BYTE);
        else if (type->get_size() == 4)
            builder.append_instr(context.machine_bb, MachineInstr::Tag::LD,
                                 {dst_reg, get_reg(ptr), Immediate::create(0)},
                                 MachineInstr::Suffix::WORD);
        else if (type->get_size() == 8)
            builder.append_instr(context.machine_bb, MachineInstr::Tag::LD,
                                 {dst_reg, get_reg(ptr), Immediate::create(0)},
                                 MachineInstr::Suffix::DWORD);
        else
            assert(false);
    }
}

void InstructionSelector::gen_store() {
    auto *ptr = context.IR_inst->get_operand(1);
    auto *st_val = context.IR_inst->get_operand(0);
    auto *type = context.IR_inst->get_operand(0)->get_type();
    if (type->is_float_type()) {
        builder.append_instr(
            context.machine_bb, MachineInstr::Tag::FST_S,
            {get_reg(st_val), get_reg(ptr), Immediate::create(0)});
    } else {
        if (type->get_size() == 1)
            builder.append_instr(
                context.machine_bb, MachineInstr::Tag::ST,
                {get_reg(st_val), get_reg(ptr), Immediate::create(0)},
                MachineInstr::Suffix::BYTE);
        else if (type->get_size() == 4)
            builder.append_instr(
                context.machine_bb, MachineInstr::Tag::ST,
                {get_reg(st_val), get_reg(ptr), Immediate::create(0)},
                MachineInstr::Suffix::WORD);
        else if (type->get_size() == 8)
            builder.append_instr(
                context.machine_bb, MachineInstr::Tag::ST,
                {get_reg(st_val), get_reg(ptr), Immediate::create(0)},
                MachineInstr::Suffix::DWORD);
        else
            assert(false);
    }
}

void InstructionSelector::gen_icmp() {
    auto *icmpInst = static_cast<ICmpInst *>(context.IR_inst);
    auto op1 = get_reg(icmpInst->get_operand(0));
    auto op2 = get_reg(icmpInst->get_operand(1));

    std::shared_ptr<MachineInstr> inst;
    std::shared_ptr<Register> tmp_reg1, tmp_reg2, tmp_reg3;
    std::shared_ptr<Register> dst_reg = get_reg(context.IR_inst);
    switch (icmpInst->get_instr_type()) {
    case Instruction::eq:
        tmp_reg1 = VirtualRegister::create(Register::General);
        tmp_reg2 = VirtualRegister::create(Register::General);
        tmp_reg3 = VirtualRegister::create(Register::General);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg1, op1, op2});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg2, op2, op1});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::OR,
                             {tmp_reg3, tmp_reg1, tmp_reg2});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::XORI,
                             {dst_reg, tmp_reg3, Immediate::create(0)});
        break;
    case Instruction::ne:
        tmp_reg1 = VirtualRegister::create(Register::General);
        tmp_reg2 = VirtualRegister::create(Register::General);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg1, op1, op2});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg2, op2, op1});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::OR,
                             {dst_reg, tmp_reg1, tmp_reg2});
        break;
    case Instruction::gt:
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {dst_reg, op2, op1});
        break;
    case Instruction::ge:
        tmp_reg1 = VirtualRegister::create(Register::General);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg1, op1, op2});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::XORI,
                             {dst_reg, tmp_reg1, Immediate::create(0)});
        break;
    case Instruction::lt:
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {dst_reg, op1, op2});
        break;
    case Instruction::le:
        tmp_reg1 = VirtualRegister::create(Register::General);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg1, op2, op1});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::XORI,
                             {dst_reg, tmp_reg1, Immediate::create(1)});
        break;
    default:
        assert(false);
    }
}

void InstructionSelector::gen_fcmp() {
    auto *fcmpInst = static_cast<FCmpInst *>(context.IR_inst);
    auto op1 = get_reg(fcmpInst->get_operand(0));
    auto op2 = get_reg(fcmpInst->get_operand(1));
    auto tmp_fccreg = VirtualRegister::create(Register::FloatCmp);
    // 根据指令类型生成汇编
    std::shared_ptr<MachineInstr> inst;
    switch (fcmpInst->get_instr_type()) {
    case Instruction::feq:
        builder.append_instr(context.machine_bb, MachineInstr::Tag::FCMP_SEQ_S,
                             {tmp_fccreg, op1, op2});
        break;
    case Instruction::fne:
        builder.append_instr(context.machine_bb, MachineInstr::Tag::FCMP_SNE_S,
                             {tmp_fccreg, op1, op2});
        break;
    case Instruction::fgt:
        builder.append_instr(context.machine_bb, MachineInstr::Tag::FCMP_SLT_S,
                             {tmp_fccreg, op2, op1});
        break;
    case Instruction::fge:
        builder.append_instr(context.machine_bb, MachineInstr::Tag::FCMP_SLE_S,
                             {tmp_fccreg, op2, op1});
        break;
    case Instruction::flt:
        builder.append_instr(context.machine_bb, MachineInstr::Tag::FCMP_SLT_S,
                             {tmp_fccreg, op1, op2});
        break;
    case Instruction::fle:
        builder.append_instr(context.machine_bb, MachineInstr::Tag::FCMP_SLE_S,
                             {tmp_fccreg, op1, op2});
        break;
    default:
        assert(false);
    }
    auto dst_reg = get_reg(context.IR_inst);
    builder.append_instr(context.machine_bb, MachineInstr::Tag::MOVCF2GR,
                         {dst_reg, tmp_fccreg});
}

void InstructionSelector::gen_phi() {
    auto *phiInst = static_cast<PhiInst *>(context.IR_inst);
    auto dst_reg = get_reg(phiInst);
    if (phiInst->get_type()->is_integer_type() ||
        phiInst->get_type()->is_pointer_type()) {
        auto tmp_reg = VirtualRegister::create(Register::General);
        for (unsigned i = 0; i < phiInst->get_num_operand(); i += 2) {
            auto *op = phiInst->get_operand(i);
            auto *bb_prev =
                static_cast<BasicBlock *>(phiInst->get_operand(i + 1));
            auto machine_bb_prev = bb_map[bb_prev];
            builder.insert_instr_before_b(machine_bb_prev,
                                          MachineInstr::Tag::MOV,
                                          {tmp_reg, get_reg(op)});
        }
        builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                             {dst_reg, tmp_reg});
    } else if (phiInst->get_type()->is_float_type()) {
        auto tmp_freg = VirtualRegister::create(Register::Float);
        for (unsigned i = 0; i < phiInst->get_num_operand(); i += 2) {
            auto *op = phiInst->get_operand(i);
            auto *bb_prev =
                static_cast<BasicBlock *>(phiInst->get_operand(i + 1));
            auto machine_bb_prev = bb_map[bb_prev];
            builder.insert_instr_before_b(machine_bb_prev,
                                          MachineInstr::Tag::MOV,
                                          {tmp_freg, get_reg(op)});
        }
        builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                             {dst_reg, tmp_freg});
    } else
        assert(false);
}

void InstructionSelector::gen_call() {
    auto *callInst = static_cast<CallInst *>(context.IR_inst);
    auto *callee = callInst->get_operand(0);
    auto *func = static_cast<Function *>(callee);
    auto mf = func_map[func];
    builder.add_int_to_reg(context.machine_bb, PhysicalRegister::sp(),
                           PhysicalRegister::sp(), -mf->params_size);
    int cnt = 0;
    for (auto &arg : func->get_args()) {
        auto *arg_val = callInst->get_operand(++cnt);
        auto suffix = (arg_val->get_type()->get_size() == 8)
                          ? MachineInstr::Suffix::DWORD
                          : MachineInstr::Suffix::WORD;
        if (mf->params_schedule_map[&arg].on_stack) {
            if (arg_val->get_type()->is_integer_type() ||
                arg_val->get_type()->is_pointer_type())
                builder.store_to_stack(context.machine_bb, get_reg(arg_val),
                                       PhysicalRegister::sp(),
                                       mf->params_schedule_map[&arg].offset,
                                       suffix);
            else if (arg_val->get_type()->is_float_type())
                builder.store_to_stack(context.machine_bb, get_reg(arg_val),
                                       PhysicalRegister::sp(),
                                       mf->params_schedule_map[&arg].offset);
            else
                assert(false);
        } else {
            if (arg_val->get_type()->is_integer_type() ||
                arg_val->get_type()->is_pointer_type())
                builder.append_instr(
                    context.machine_bb, MachineInstr::Tag::MOV,
                    {mf->params_schedule_map[&arg].reg, get_reg(arg_val)});
            else if (arg_val->get_type()->is_float_type())
                builder.append_instr(
                    context.machine_bb, MachineInstr::Tag::MOV,
                    {mf->params_schedule_map[&arg].reg, get_reg(arg_val)});
            else
                assert(false);
        }
    }

    builder.append_instr(context.machine_bb, MachineInstr::Tag::BL,
                         {std::make_shared<Label>(mf)});

    builder.add_int_to_reg(context.machine_bb, PhysicalRegister::sp(),
                           PhysicalRegister::sp(), mf->params_size);
    if (not callInst->get_type()->is_void_type()) {
        if (callInst->get_type()->is_integer_type()) {
            auto dst_reg = get_reg(callInst);
            builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                                 {dst_reg, PhysicalRegister::a(0)});
        } else if (callInst->get_type()->is_float_type()) {
            auto dst_freg = get_reg(callInst);
            builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                                 {dst_freg, PhysicalRegister::fa(0)});
        }
    }
}

void InstructionSelector::gen_gep() {
    auto *gepInst = static_cast<GetElementPtrInst *>(context.IR_inst);
    auto *ptr = gepInst->get_operand(0);
    auto *type = ptr->get_type();
    auto pre = get_reg(ptr);
    auto dst_reg = get_reg(gepInst);
    for (unsigned i = 1; i < gepInst->get_num_operand(); i++) {
        auto *idx = gepInst->get_operand(i);
        unsigned int size = 0;
        if (type->is_pointer_type()) {
            size = type->get_pointer_element_type()->get_size();
            type = type->get_pointer_element_type();

        } else if (type->is_array_type()) {
            size = type->get_array_element_type()->get_size();
            type = type->get_array_element_type();
        } else
            size = type->get_size();
        auto tmp_reg1 = VirtualRegister::create(Register::General);
        auto tmp_reg2 = VirtualRegister::create(Register::General);
        auto tmp_reg3 = VirtualRegister::create(Register::General);
        builder.load_int32(context.machine_bb, size, tmp_reg1);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::MUL,
                             {tmp_reg2, get_reg(idx), tmp_reg1},
                             MachineInstr::Suffix::WORD);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::ADD,
                             {tmp_reg3, pre, tmp_reg2},
                             MachineInstr::Suffix::DWORD);
        pre = tmp_reg3;
    }
    builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                         {dst_reg, pre});
}

void InstructionSelector::gen_zext() {
    auto *zextInst = static_cast<ZextInst *>(context.IR_inst);
    auto *op = zextInst->get_operand(0);
    auto dst_reg = get_reg(zextInst);
    builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                         {dst_reg, get_reg(op)});
}

void InstructionSelector::gen_sitofp() {
    // 整数转向浮点数
    auto *sitofpInst = static_cast<SiToFpInst *>(context.IR_inst);
    auto *op = sitofpInst->get_operand(0);
    auto dst_freg = get_reg(sitofpInst);
    auto tmp_freg = VirtualRegister::create(Register::Float);
    builder.append_instr(context.machine_bb, MachineInstr::Tag::MOVGR2FR_W,
                         {tmp_freg, get_reg(op)});
    builder.append_instr(context.machine_bb, MachineInstr::Tag::FFINT_S_W,
                         {dst_freg, tmp_freg});
}

void InstructionSelector::gen_fptosi() {
    // 浮点数转向整数，注意向下取整(round to zero)
    auto *fptosiInst = static_cast<FpToSiInst *>(context.IR_inst);
    auto *op = fptosiInst->get_operand(0);
    auto tmp_freg = VirtualRegister::create(Register::Float);
    auto dst_reg = get_reg(fptosiInst);
    builder.append_instr(context.machine_bb, MachineInstr::Tag::FTINTRZ_W_S,
                         {tmp_freg, get_reg(op)});
    builder.append_instr(context.machine_bb, MachineInstr::Tag::MOVFR2GR_S,
                         {dst_reg, tmp_freg});
}

void InstructionSelector::gen_bitcast() {
    // bitcast 指令
    auto *bitcastInst = static_cast<BitCastInst *>(context.IR_inst);
    auto *op = bitcastInst->get_operand(0);
    auto dst_reg = get_reg(bitcastInst);
    builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                         {dst_reg, get_reg(op)});
}

void InstructionSelector::gen_inst() {
    switch (context.IR_inst->get_instr_type()) {
    case Instruction::ret:
        gen_ret();
        break;
    case Instruction::br:
        gen_br();
        break;
    case Instruction::add:
    case Instruction::sub:
    case Instruction::mul:
    case Instruction::sdiv:
    case Instruction::srem:
        gen_binary();
        break;
    case Instruction::fadd:
    case Instruction::fsub:
    case Instruction::fmul:
    case Instruction::fdiv:
        gen_float_binary();
        break;
    case Instruction::alloca:
        gen_alloca();
        break;
    case Instruction::load:
        gen_load();
        break;
    case Instruction::store:
        gen_store();
        break;
    case Instruction::ge:
    case Instruction::gt:
    case Instruction::le:
    case Instruction::lt:
    case Instruction::eq:
    case Instruction::ne:
        gen_icmp();
        break;
    case Instruction::fge:
    case Instruction::fgt:
    case Instruction::fle:
    case Instruction::flt:
    case Instruction::feq:
    case Instruction::fne:
        gen_fcmp();
        break;
    case Instruction::phi:
        gen_phi();
        break;
    case Instruction::call:
        gen_call();
        break;
    case Instruction::getelementptr:
        gen_gep();
        break;
    case Instruction::zext:
        gen_zext();
        break;
    case Instruction::fptosi:
        gen_fptosi();
        break;
    case Instruction::sitofp:
        gen_sitofp();
        break;
    case Instruction::bitcast:
        gen_bitcast();
        break;
    }
}