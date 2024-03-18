#include "GlobalVariable.hpp"
#include "Instruction.hpp"
#include "InstructionSelector.hpp"
#include "MIBuilder.hpp"
#include "MachineBasicBlock.hpp"
#include "MachineInstr.hpp"
#include "Module.hpp"
#include "Operand.hpp"

#include <memory>
void InstructionSelector::run() {
    // build module, functions and basic blocks
    IR_module = module->get_IR_module();
    for (auto &func : IR_module->get_functions()) {
        auto machine_func = std::make_shared<MachineFunction>(&func, module);
        module->add_function(machine_func);
        func_map[&func] = machine_func;
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

        machine_func->set_prologue_block(
            std::make_shared<MachineBasicBlock>(nullptr, machine_func, ""));
        machine_func->set_epilogue_block(std::make_shared<MachineBasicBlock>(
            nullptr, machine_func, "_exit"));
    }

    // build CFG
    for (auto &func : IR_module->get_functions()) {
        auto entry_mbb = bb_map[func.get_entry_block()];
        func_map[&func]->get_prologue_block()->add_succ_basic_block(entry_mbb);
        entry_mbb->add_pre_basic_block(func_map[&func]->get_prologue_block());

        for (auto &bb : func.get_basic_blocks()) {
            context.machine_bb = bb_map[&bb];
            for (auto &pred : bb.get_pre_basic_blocks()) {
                context.machine_bb->add_pre_basic_block(bb_map[pred]);
            }
            for (auto &succ : bb.get_succ_basic_blocks()) {
                context.machine_bb->add_succ_basic_block(bb_map[succ]);
            }
            if (bb.get_instructions().back().get_instr_type() ==
                Instruction::ret) {
                context.machine_bb->add_succ_basic_block(
                    func_map[&func]->get_epilogue_block());
            }
        }
    }

    // select instructions
    for (auto &machine_func : module->get_functions()) {
        context.clear();
        context.machine_func = machine_func;

        gen_store_params();
        for (auto &bb : machine_func->get_basic_blocks()) {
            context.machine_bb = bb;

            for (auto &inst : bb->get_IR_basic_block()->get_instructions()) {
                context.IR_inst = &inst;
                gen_inst();
            }
        }
        builder.gen_prologue_epilogue(machine_func);
    }
}

void InstructionSelector::gen_store_params() {

    auto entry_mbb =
        bb_map[context.machine_func->get_IR_function()->get_entry_block()];
    for (auto &arg : context.machine_func->get_IR_function()->get_args()) {
        if (arg.get_type()->is_integer_type() ||
            arg.get_type()->is_pointer_type()) {
            auto tmp_reg = VirtualRegister::create(Register::General);
            if (context.machine_func->params_schedule_map[&arg].on_stack) {
                builder.load_from_stack(
                    entry_mbb, tmp_reg, PhysicalRegister::fp(),
                    context.machine_func->params_schedule_map[&arg].offset);
                context.val_map[&arg] = tmp_reg;
            } else {
                auto tmp_reg = VirtualRegister::create(Register::General);
                builder.append_instr(
                    entry_mbb, MachineInstr::Tag::MOV,
                    {tmp_reg,
                     context.machine_func->params_schedule_map[&arg].reg});
            }
            context.val_map[&arg] = tmp_reg;
        } else if (arg.get_type()->is_float_type()) {
            auto tmp_freg = VirtualRegister::create(Register::Float);
            if (context.machine_func->params_schedule_map[&arg].on_stack) {
                builder.load_from_stack(
                    entry_mbb, tmp_freg, PhysicalRegister::fp(),
                    context.machine_func->params_schedule_map[&arg].offset);
                context.val_map[&arg] = tmp_freg;
            } else {
                auto tmp_freg = VirtualRegister::create(Register::Float);
                builder.append_instr(
                    entry_mbb, MachineInstr::Tag::MOV,
                    {tmp_freg,
                     context.machine_func->params_schedule_map[&arg].reg});
            }
            context.val_map[&arg] = tmp_freg;
        } else
            assert(false);
    }
}

void InstructionSelector::gen_ret() {
    auto *retInst = static_cast<ReturnInst *>(context.IR_inst);
    if (retInst->get_num_operand() > 0) {
        auto *ret_val = retInst->get_operand(0);
        if (ret_val->get_type()->is_integer_type())
            builder.append_instr(
                context.machine_bb, MachineInstr::Tag::MOV,
                {PhysicalRegister::a(0), context.val_map[ret_val]});
        else if (ret_val->get_type()->is_float_type())
            builder.append_instr(
                context.machine_bb, MachineInstr::Tag::MOV,
                {PhysicalRegister::fa(0), context.val_map[ret_val]});
        else
            assert(false);
    } else
        builder.append_instr(
            context.machine_bb, MachineInstr::Tag::MOV,
            {PhysicalRegister::a(0), PhysicalRegister::zero()});
    builder.append_instr(
        context.machine_bb, MachineInstr::Tag::B,
        {std::make_shared<Label>(context.machine_func->get_epilogue_block())});
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
            {context.val_map[cond], std::make_shared<Label>(bb_map[truebb])});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::B,
                             {std::make_shared<Label>(bb_map[falsebb])});
    } else {
        auto *branchbb = static_cast<BasicBlock *>(branchInst->get_operand(0));
        builder.append_instr(context.machine_bb, MachineInstr::Tag::B,
                             {std::make_shared<Label>(bb_map[branchbb])});
    }
}

void InstructionSelector::gen_binary() {
    auto op1 = context.val_map[context.IR_inst->get_operand(0)];
    auto op2 = context.val_map[context.IR_inst->get_operand(1)];
    // 根据指令类型生成汇编
    std::shared_ptr<MachineInstr> inst;
    auto tmp_reg = VirtualRegister::create(Register::General);
    switch (context.IR_inst->get_instr_type()) {
    case Instruction::add:
        inst = builder.append_instr(context.machine_bb, MachineInstr::Tag::ADD,
                                    {tmp_reg, op1, op2},
                                    MachineInstr::Suffix::WORD);
        break;
    case Instruction::sub:
        inst = builder.append_instr(context.machine_bb, MachineInstr::Tag::SUB,
                                    {tmp_reg, op1, op2},
                                    MachineInstr::Suffix::WORD);
        break;
    case Instruction::mul:
        inst = builder.append_instr(context.machine_bb, MachineInstr::Tag::MUL,
                                    {tmp_reg, op1, op2},
                                    MachineInstr::Suffix::WORD);
        break;
    case Instruction::sdiv:
        inst = builder.append_instr(context.machine_bb, MachineInstr::Tag::DIV,
                                    {tmp_reg, op1, op2},
                                    MachineInstr::Suffix::WORD);
        break;
    case Instruction::srem:
        inst = builder.append_instr(context.machine_bb, MachineInstr::Tag::MOD,
                                    {tmp_reg, op1, op2},
                                    MachineInstr::Suffix::WORD);
        break;
    default:
        assert(false);
    }
    // 将结果填入栈帧中
    context.val_map[context.IR_inst] = tmp_reg;
}

void InstructionSelector::gen_float_binary() {
    auto op1 = context.val_map[context.IR_inst->get_operand(0)];
    auto op2 = context.val_map[context.IR_inst->get_operand(1)];
    // 根据指令类型生成汇编
    std::shared_ptr<MachineInstr> inst;
    auto tmp_freg = VirtualRegister::create(Register::Float);
    switch (context.IR_inst->get_instr_type()) {
    case Instruction::fadd:
        inst =
            builder.append_instr(context.machine_bb, MachineInstr::Tag::FADD_S,
                                 {tmp_freg, op1, op2});
        break;
    case Instruction::fsub:
        inst =
            builder.append_instr(context.machine_bb, MachineInstr::Tag::FSUB_S,
                                 {tmp_freg, op1, op2});
        break;
    case Instruction::fmul:
        inst =
            builder.append_instr(context.machine_bb, MachineInstr::Tag::FMUL_S,
                                 {tmp_freg, op1, op2});
        break;
    case Instruction::fdiv:
        inst =
            builder.append_instr(context.machine_bb, MachineInstr::Tag::FDIV_S,
                                 {tmp_freg, op1, op2});
        break;
    default:
        assert(false);
    }
    // 将结果填入栈帧中
    context.val_map[context.IR_inst] = tmp_freg;
}

void InstructionSelector::gen_alloca() {
    auto *allocaInst = static_cast<AllocaInst *>(context.IR_inst);
    context.machine_func->frame_scheduler->insert_alloca(allocaInst);
    auto tmp_reg = VirtualRegister::create(Register::General);

    builder.add_int_to_reg(
        context.machine_bb, tmp_reg, PhysicalRegister::fp(),
        -context.machine_func->frame_scheduler->get_alloca_offset(allocaInst));
    context.val_map[context.IR_inst] = tmp_reg;
}

void InstructionSelector::gen_load() {
    auto *ptr = context.IR_inst->get_operand(0);
    auto *type = context.IR_inst->get_type();
    auto ld_pos = context.val_map[ptr];
    if (dynamic_cast<GlobalVariable *>(ptr) != nullptr) {
        auto tmp_reg = VirtualRegister::create(Register::General);
        builder.append_instr(
            context.machine_bb, MachineInstr::Tag::LA_LOCAL,
            {tmp_reg, std::make_shared<Label>(ptr->get_name())});
        ld_pos = tmp_reg;
    }
    if (type->is_float_type()) {
        auto tmp_freg = VirtualRegister::create(Register::Float);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::FLD_S,
                             {tmp_freg, ld_pos, Immediate::create(0)});
        context.val_map[context.IR_inst] = tmp_freg;
    } else {
        auto tmp_reg = VirtualRegister::create(Register::General);
        // load 整数类型的数据
        if (type->get_size() == 1)
            builder.append_instr(context.machine_bb, MachineInstr::Tag::LD,
                                 {tmp_reg, ld_pos, Immediate::create(0)},
                                 MachineInstr::Suffix::BYTE);
        else if (type->get_size() == 4)
            builder.append_instr(
                context.machine_bb, MachineInstr::Tag::LD,
                {tmp_reg, context.val_map[ptr], Immediate::create(0)},
                MachineInstr::Suffix::WORD);
        else if (type->get_size() == 8)
            builder.append_instr(
                context.machine_bb, MachineInstr::Tag::LD,
                {tmp_reg, context.val_map[ptr], Immediate::create(0)},
                MachineInstr::Suffix::DWORD);
        else
            assert(false);
        context.val_map[context.IR_inst] = tmp_reg;
    }
}

void InstructionSelector::gen_store() {
    auto *ptr = context.IR_inst->get_operand(1);
    auto *st_val = context.IR_inst->get_operand(0);
    auto *type = context.IR_inst->get_operand(0)->get_type();
    auto st_pos = context.val_map[ptr];
    if (dynamic_cast<GlobalVariable *>(ptr) != nullptr) {
        auto tmp_reg = VirtualRegister::create(Register::General);
        builder.append_instr(
            context.machine_bb, MachineInstr::Tag::LA_LOCAL,
            {tmp_reg, std::make_shared<Label>(ptr->get_name())});
        st_pos = tmp_reg;
    }

    if (type->is_float_type()) {
        builder.append_instr(
            context.machine_bb, MachineInstr::Tag::FST_S,
            {context.val_map[st_val], st_pos, Immediate::create(0)});
    } else {
        if (type->get_size() == 1)
            builder.append_instr(
                context.machine_bb, MachineInstr::Tag::ST,
                {context.val_map[st_val], st_pos, Immediate::create(0)},
                MachineInstr::Suffix::BYTE);
        else if (type->get_size() == 4)
            builder.append_instr(
                context.machine_bb, MachineInstr::Tag::ST,
                {context.val_map[st_val], st_pos, Immediate::create(0)},
                MachineInstr::Suffix::WORD);
        else if (type->get_size() == 8)
            builder.append_instr(
                context.machine_bb, MachineInstr::Tag::ST,
                {context.val_map[st_val], st_pos, Immediate::create(0)},
                MachineInstr::Suffix::DWORD);
        else
            assert(false);
    }
}

void InstructionSelector::gen_icmp() {
    auto *icmpInst = static_cast<ICmpInst *>(context.IR_inst);
    auto op1 = context.val_map[icmpInst->get_operand(0)];
    auto op2 = context.val_map[icmpInst->get_operand(1)];
    // 根据指令类型生成汇编
    std::shared_ptr<MachineInstr> inst;
    std::shared_ptr<Register> tmp_reg1, tmp_reg2, tmp_reg3, tmp_reg4;
    switch (icmpInst->get_instr_type()) {
    case Instruction::eq:
        tmp_reg1 = VirtualRegister::create(Register::General);
        tmp_reg2 = VirtualRegister::create(Register::General);
        tmp_reg3 = VirtualRegister::create(Register::General);
        tmp_reg4 = VirtualRegister::create(Register::General);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg1, op1, op2});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg2, op2, op1});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::OR,
                             {tmp_reg3, tmp_reg1, tmp_reg2});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::XORI,
                             {tmp_reg4, tmp_reg3, Immediate::create(0)});
        context.val_map[context.IR_inst] = tmp_reg4;
        break;
    case Instruction::ne:
        tmp_reg1 = VirtualRegister::create(Register::General);
        tmp_reg2 = VirtualRegister::create(Register::General);
        tmp_reg3 = VirtualRegister::create(Register::General);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg1, op1, op2});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg2, op2, op1});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::OR,
                             {tmp_reg3, tmp_reg1, tmp_reg2});
        context.val_map[context.IR_inst] = tmp_reg3;
        break;
    case Instruction::gt:
        tmp_reg1 = VirtualRegister::create(Register::General);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg1, op2, op1});
        context.val_map[context.IR_inst] = tmp_reg1;
        break;
    case Instruction::ge:
        tmp_reg1 = VirtualRegister::create(Register::General);
        tmp_reg2 = VirtualRegister::create(Register::General);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg1, op1, op2});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::XORI,
                             {tmp_reg2, tmp_reg1, Immediate::create(0)});
        context.val_map[context.IR_inst] = tmp_reg2;
        break;
    case Instruction::lt:
        tmp_reg1 = VirtualRegister::create(Register::General);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg1, op1, op2});
        context.val_map[context.IR_inst] = tmp_reg1;
        break;
    case Instruction::le:
        tmp_reg1 = VirtualRegister::create(Register::General);
        tmp_reg2 = VirtualRegister::create(Register::General);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::SLT,
                             {tmp_reg1, op2, op1});
        builder.append_instr(context.machine_bb, MachineInstr::Tag::XORI,
                             {tmp_reg2, tmp_reg1, Immediate::create(1)});
        context.val_map[context.IR_inst] = tmp_reg2;
        break;
    default:
        assert(false);
    }
}

void InstructionSelector::gen_fcmp() {
    auto *fcmpInst = static_cast<FCmpInst *>(context.IR_inst);
    auto op1 = context.val_map[fcmpInst->get_operand(0)];
    auto op2 = context.val_map[fcmpInst->get_operand(1)];
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
    auto tmp_reg = VirtualRegister::create(Register::General);
    builder.append_instr(context.machine_bb, MachineInstr::Tag::MOVCF2GR,
                         {tmp_reg, tmp_fccreg});
    context.val_map[context.IR_inst] = tmp_reg;
}

void InstructionSelector::gen_phi() {
    auto *phiInst = static_cast<PhiInst *>(context.IR_inst);
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
                                          {tmp_reg, context.val_map[op]});
        }
        auto phi_reg = VirtualRegister::create(Register::General);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                             {phi_reg, tmp_reg});
        context.val_map[phiInst] = phi_reg;
    } else if (phiInst->get_type()->is_float_type()) {
        auto tmp_freg = VirtualRegister::create(Register::Float);
        for (unsigned i = 0; i < phiInst->get_num_operand(); i += 2) {
            auto *op = phiInst->get_operand(i);
            auto *bb_prev =
                static_cast<BasicBlock *>(phiInst->get_operand(i + 1));
            auto machine_bb_prev = bb_map[bb_prev];
            builder.insert_instr_before_b(machine_bb_prev,
                                          MachineInstr::Tag::MOV,
                                          {tmp_freg, context.val_map[op]});
        }
        auto phi_freg = VirtualRegister::create(Register::Float);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                             {phi_freg, tmp_freg});
        context.val_map[phiInst] = phi_freg;
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
                builder.store_to_stack(
                    context.machine_bb, context.val_map[arg_val],
                    PhysicalRegister::sp(),
                    mf->params_schedule_map[&arg].offset, suffix);
            else if (arg_val->get_type()->is_float_type())
                builder.store_to_stack(context.machine_bb,
                                       context.val_map[arg_val],
                                       PhysicalRegister::sp(),
                                       mf->params_schedule_map[&arg].offset);
            else
                assert(false);
        } else {
            if (arg_val->get_type()->is_integer_type() ||
                arg_val->get_type()->is_pointer_type())
                builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                                     {mf->params_schedule_map[&arg].reg,
                                      context.val_map[arg_val]});
            else if (arg_val->get_type()->is_float_type())
                builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                                     {mf->params_schedule_map[&arg].reg,
                                      context.val_map[arg_val]});
            else
                assert(false);
        }
    }
    std::unordered_map<std::shared_ptr<PhysicalRegister>,
                       std::shared_ptr<VirtualRegister>>
        reg_map;
    reg_map.clear();
    for (auto reg : PhysicalRegister::caller_saved_regs()) {
        if (reg->get_type() == Register::General)
            reg_map[reg] = VirtualRegister::create(Register::General);
        else if (reg->get_type() == Register::Float)
            reg_map[reg] = VirtualRegister::create(Register::Float);
        else if (reg->get_type() == Register::FloatCmp)
            reg_map[reg] = VirtualRegister::create(Register::FloatCmp);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                             {reg_map[reg], reg});
    }
    builder.append_instr(context.machine_bb, MachineInstr::Tag::BL,
                         {std::make_shared<Label>(mf->get_prologue_block())});
    for (auto reg : PhysicalRegister::caller_saved_regs()) {
        builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                             {reg, reg_map[reg]});
    }
    builder.add_int_to_reg(context.machine_bb, PhysicalRegister::sp(),
                           PhysicalRegister::sp(), mf->params_size);
    if (not callInst->get_type()->is_void_type()) {
        if (callInst->get_type()->is_integer_type()) {
            auto tmp_reg = VirtualRegister::create(Register::General);
            builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                                 {tmp_reg, PhysicalRegister::a(0)});
            context.val_map[callInst] = tmp_reg;
        } else if (callInst->get_type()->is_float_type()) {
            auto tmp_freg = VirtualRegister::create(Register::Float);
            builder.append_instr(context.machine_bb, MachineInstr::Tag::MOV,
                                 {tmp_freg, PhysicalRegister::fa(0)});
            context.val_map[callInst] = tmp_freg;
        }
    }
}

void InstructionSelector::gen_gep() {
    auto *gepInst = static_cast<GetElementPtrInst *>(context.IR_inst);
    auto *ptr = gepInst->get_operand(0);
    auto *type = ptr->get_type();
    auto pre = context.val_map[ptr];
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
        if (Immediate::create(size)->is_imm_length(12)) {
            builder.append_instr(
                context.machine_bb, MachineInstr::Tag::ADDI,
                {tmp_reg1, PhysicalRegister::zero(), Immediate::create(size)},
                MachineInstr::Suffix::WORD);
        } else {
            builder.load_large_int32(context.machine_bb, size, tmp_reg1);
        }
        builder.append_instr(context.machine_bb, MachineInstr::Tag::MUL,
                             {tmp_reg2, context.val_map[idx], tmp_reg1},
                             MachineInstr::Suffix::WORD);
        builder.append_instr(context.machine_bb, MachineInstr::Tag::ADD,
                             {tmp_reg3, pre, tmp_reg2},
                             MachineInstr::Suffix::DWORD);
        pre = tmp_reg3;
    }
    context.val_map[gepInst] = pre;
}

void InstructionSelector::gen_zext() {
    auto *zextInst = static_cast<ZextInst *>(context.IR_inst);
    auto *op = zextInst->get_operand(0);
    context.val_map[zextInst] = context.val_map[op];
}

void InstructionSelector::gen_sitofp() {
    // 整数转向浮点数
    auto *sitofpInst = static_cast<SiToFpInst *>(context.IR_inst);
    auto *op = sitofpInst->get_operand(0);
    auto tmp_freg1 = VirtualRegister::create(Register::Float);
    auto tmp_freg2 = VirtualRegister::create(Register::Float);
    builder.append_instr(context.machine_bb, MachineInstr::Tag::MOVGR2FR_W,
                         {tmp_freg1, context.val_map[op]});
    builder.append_instr(context.machine_bb, MachineInstr::Tag::FFINT_S_W,
                         {tmp_freg2, tmp_freg1});
    context.val_map[context.IR_inst] = tmp_freg2;
}

void InstructionSelector::gen_fptosi() {
    // 浮点数转向整数，注意向下取整(round to zero)
    auto *fptosiInst = static_cast<FpToSiInst *>(context.IR_inst);
    auto *op = fptosiInst->get_operand(0);
    auto tmp_freg = VirtualRegister::create(Register::Float);
    auto tmp_reg = VirtualRegister::create(Register::General);
    builder.append_instr(context.machine_bb, MachineInstr::Tag::FTINTRZ_W_S,
                         {tmp_freg, context.val_map[op]});
    builder.append_instr(context.machine_bb, MachineInstr::Tag::MOVFR2GR_S,
                         {tmp_reg, tmp_freg});
    context.val_map[context.IR_inst] = tmp_reg;
}

void InstructionSelector::gen_bitcast() {
    // bitcast 指令
    auto *bitcastInst = static_cast<BitCastInst *>(context.IR_inst);
    auto *op = bitcastInst->get_operand(0);
    context.val_map[context.IR_inst] = context.val_map[op];
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