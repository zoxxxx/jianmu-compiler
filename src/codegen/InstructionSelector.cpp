#include "Instruction.hpp"
#include "InstructionSelector.hpp"
#include "MachineInstr.hpp"
#include "Module.hpp"
#include "Operand.hpp"
#include <memory>
void InstructionSelector::run() {
    // build module, functions and basic blocks
    module = std::make_shared<MachineModule>(IR_module);
    for (auto &func : IR_module->get_functions()) {
        auto machine_func = std::make_shared<MachineFunction>(func, module);
        module->add_function(machine_func);
        func_map[&func] = machine_func;
        for (auto &bb : func.get_basic_blocks()) {
            auto machine_bb =
                std::make_shared<MachineBasicBlock>(bb, machine_func);
            machine_func->add_basic_block(machine_bb);
            bb_map[&bb] = machine_bb;
            context.machine_bb = machine_bb;
            for (auto &inst : bb.get_instructions()) {
                context.IR_inst = &inst;
            }
        }

        machine_func->set_entry_block(
            std::make_shared<MachineBasicBlock>(nullptr, machine_func, ""));
        machine_func->set_exit_block(std::make_shared<MachineBasicBlock>(
            nullptr, machine_func, "_exit"));
    }

    // build CFG
    for (auto &func : IR_module->get_functions()) {
        func_map[&func]->get_entry_block()->add_succ_basic_block(
            bb_map[func.get_entry_block()]);
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
                    func_map[&func]->get_exit_block());
            }
        }
    }

    // select instructions
    for (auto &machine_func : module->get_functions()) {
        context.clear();
        context.machine_func = machine_func;
        for (auto &bb : machine_func->get_basic_blocks()) {
            context.machine_bb = bb;

            for (auto &inst : bb->get_IR_basic_block()->get_instructions()) {
                context.IR_inst = &inst;
                gen_inst();
            }
        }
    }
}

void InstructionSelector::gen_ret() {
    auto *retInst = static_cast<ReturnInst *>(context.IR_inst);
    if (retInst->get_num_operand() > 0) {
        auto *ret_val = retInst->get_operand(0);
        if (ret_val->get_type()->is_integer_type())
            builder.gen_instr(
                context.machine_bb, MachineInstr::Tag::MOV,
                {PhysicalRegister::a(0), context.val_map[ret_val]});
        else if (ret_val->get_type()->is_float_type())
            builder.gen_instr(
                context.machine_bb, MachineInstr::Tag::MOV,
                {PhysicalRegister::fa(0), context.val_map[ret_val]});
        else
            assert(false);
    } else
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::MOV,
                          {PhysicalRegister::a(0), PhysicalRegister::zero()});
    builder.gen_instr(
        context.machine_bb, MachineInstr::Tag::B,
        {std::make_shared<Label>(context.machine_func->get_exit_block())});
}

void InstructionSelector::gen_br() {
    auto *branchInst = static_cast<BranchInst *>(context.IR_inst);
    if (branchInst->is_cond_br()) {
        auto *cond = branchInst->get_operand(0);
        auto *truebb = static_cast<BasicBlock *>(branchInst->get_operand(1));
        auto *falsebb = static_cast<BasicBlock *>(branchInst->get_operand(2));

        auto tmp_reg = VirtualRegister::create(Register::General);
        builder.gen_instr(
            context.machine_bb, MachineInstr::Tag::BNEZ,
            {context.val_map[cond], std::make_shared<Label>(bb_map[truebb])});
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::B,
                          {std::make_shared<Label>(bb_map[falsebb])});
    } else {
        auto *branchbb = static_cast<BasicBlock *>(branchInst->get_operand(0));
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::B,
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
        inst =
            builder.gen_instr(context.machine_bb, MachineInstr::Tag::ADD,
                              {tmp_reg, op1, op2}, MachineInstr::Suffix::WORD);
        break;
    case Instruction::sub:
        inst =
            builder.gen_instr(context.machine_bb, MachineInstr::Tag::SUB,
                              {tmp_reg, op1, op2}, MachineInstr::Suffix::WORD);
        break;
    case Instruction::mul:
        inst =
            builder.gen_instr(context.machine_bb, MachineInstr::Tag::MUL,
                              {tmp_reg, op1, op2}, MachineInstr::Suffix::WORD);
        break;
    case Instruction::sdiv:
        inst =
            builder.gen_instr(context.machine_bb, MachineInstr::Tag::DIV,
                              {tmp_reg, op1, op2}, MachineInstr::Suffix::WORD);
        break;
    case Instruction::srem:
        inst =
            builder.gen_instr(context.machine_bb, MachineInstr::Tag::MOD,
                              {tmp_reg, op1, op2}, MachineInstr::Suffix::WORD);
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
        inst = builder.gen_instr(context.machine_bb, MachineInstr::Tag::FADD_S,
                                 {tmp_freg, op1, op2});
        break;
    case Instruction::fsub:
        inst = builder.gen_instr(context.machine_bb, MachineInstr::Tag::FSUB_S,
                                 {tmp_freg, op1, op2});
        break;
    case Instruction::fmul:
        inst = builder.gen_instr(context.machine_bb, MachineInstr::Tag::FMUL_S,
                                 {tmp_freg, op1, op2});
        break;
    case Instruction::fdiv:
        inst = builder.gen_instr(context.machine_bb, MachineInstr::Tag::FDIV_S,
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
    auto offset =
        context.machine_func->frame_scheduler->get_alloca_offset(allocaInst);
    auto tmp_reg = VirtualRegister::create(Register::General);
    builder.add_int_to_reg(context.machine_bb, tmp_reg, PhysicalRegister::sp(),
                           -offset);
    context.val_map[context.IR_inst] = tmp_reg;
}

void InstructionSelector::gen_load() {
    auto *ptr = context.IR_inst->get_operand(0);
    auto *type = context.IR_inst->get_type();

    if (type->is_float_type()) {
        auto tmp_freg = VirtualRegister::create(Register::Float);
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::FLD_S,
                          {tmp_freg, context.val_map[ptr]});
        context.val_map[context.IR_inst] = tmp_freg;
    } else {
        auto tmp_reg = VirtualRegister::create(Register::General);
        // load 整数类型的数据
        if (type->get_size() == 1)
            builder.gen_instr(context.machine_bb, MachineInstr::Tag::LD,
                              {tmp_reg, context.val_map[ptr], 0},
                              MachineInstr::Suffix::BYTE);
        else if (type->get_size() == 4)
            builder.gen_instr(context.machine_bb, MachineInstr::Tag::LD,
                              {tmp_reg, context.val_map[ptr], 0},
                              MachineInstr::Suffix::WORD);
        else if (type->get_size() == 8)
            builder.gen_instr(context.machine_bb, MachineInstr::Tag::LD,
                              {tmp_reg, context.val_map[ptr], 0},
                              MachineInstr::Suffix::DWORD);
        else
            assert(false);
        context.val_map[context.IR_inst] = tmp_reg;
    }
}

void InstructionSelector::gen_store() {
    // 翻译 store 指令
    auto *ptr = context.IR_inst->get_operand(1);
    auto *st_val = context.IR_inst->get_operand(0);
    auto *type = context.IR_inst->get_operand(0)->get_type();

    if (type->is_float_type()) {
        auto tmp_freg = VirtualRegister::create(Register::Float);
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::FST_S,
                          {context.val_map[st_val], context.val_map[ptr],
                           std::make_shared<Immediate>(0)});
    } else {
        if (type->get_size() == 1)
            builder.gen_instr(context.machine_bb, MachineInstr::Tag::ST,
                              {context.val_map[st_val], context.val_map[ptr],
                               std::make_shared<Immediate>(0)},
                              MachineInstr::Suffix::BYTE);
        else if (type->get_size() == 4)
            builder.gen_instr(context.machine_bb, MachineInstr::Tag::ST,
                              {context.val_map[st_val], context.val_map[ptr],
                               std::make_shared<Immediate>(0)},
                              MachineInstr::Suffix::WORD);
        else if (type->get_size() == 8)
            builder.gen_instr(context.machine_bb, MachineInstr::Tag::ST,
                              {context.val_map[st_val], context.val_map[ptr],
                               std::make_shared<Immediate>(0)},
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
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::SLT,
                          {tmp_reg1, op1, op2});
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::SLT,
                          {tmp_reg2, op2, op1});
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::OR,
                          {tmp_reg3, tmp_reg1, tmp_reg2});
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::XORI,
                          {tmp_reg4, tmp_reg3, std::make_shared<Immediate>(1)});
        context.val_map[context.IR_inst] = tmp_reg4;
        break;
    case Instruction::ne:
        tmp_reg1 = VirtualRegister::create(Register::General);
        tmp_reg2 = VirtualRegister::create(Register::General);
        tmp_reg3 = VirtualRegister::create(Register::General);
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::SLT,
                          {tmp_reg1, op1, op2});
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::SLT,
                          {tmp_reg2, op2, op1});
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::OR,
                          {tmp_reg3, tmp_reg1, tmp_reg2});
        context.val_map[context.IR_inst] = tmp_reg3;
        break;
    case Instruction::gt:
        tmp_reg1 = VirtualRegister::create(Register::General);
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::SLT,
                          {tmp_reg1, op2, op1});
        context.val_map[context.IR_inst] = tmp_reg1;
        break;
    case Instruction::ge:
        tmp_reg1 = VirtualRegister::create(Register::General);
        tmp_reg2 = VirtualRegister::create(Register::General);
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::SLT,
                          {tmp_reg1, op1, op2});
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::XORI,
                          {tmp_reg2, tmp_reg1, std::make_shared<Immediate>(1)});
        context.val_map[context.IR_inst] = tmp_reg2;
        break;
    case Instruction::lt:
        tmp_reg1 = VirtualRegister::create(Register::General);
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::SLT,
                          {tmp_reg1, op1, op2});
        context.val_map[context.IR_inst] = tmp_reg1;
        break;
    case Instruction::le:
        tmp_reg1 = VirtualRegister::create(Register::General);
        tmp_reg2 = VirtualRegister::create(Register::General);
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::SLT,
                          {tmp_reg1, op2, op1});
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::XORI,
                          {tmp_reg2, tmp_reg1, std::make_shared<Immediate>(1)});
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
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::FCMP_SEQ_S,
                          {tmp_fccreg, op1, op2});
        break;
    case Instruction::fne:
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::FCMP_SNE_S,
                          {tmp_fccreg, op1, op2});
        break;
    case Instruction::fgt:
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::FCMP_SLT_S,
                          {tmp_fccreg, op2, op1});
        break;
    case Instruction::fge:
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::FCMP_SLE_S,
                          {tmp_fccreg, op2, op1});
        break;
    case Instruction::flt:
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::FCMP_SLT_S,
                          {tmp_fccreg, op1, op2});
        break;
    case Instruction::fle:
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::FCMP_SLE_S,
                          {tmp_fccreg, op1, op2});
        break;
    default:
        assert(false);
    }
    auto tmp_reg = VirtualRegister::create(Register::General);
    builder.gen_instr(context.machine_bb, MachineInstr::Tag::MOVCF2GR,
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
            builder.insert_instr(
                machine_bb_prev, MachineInstr::Tag::MOV,
                {tmp_reg, context.val_map[op]},
                std::prev(machine_bb_prev->get_instrs().end()));
        }
        auto phi_reg = VirtualRegister::create(Register::General);
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::MOV,
                          {phi_reg, tmp_reg});
        context.val_map[phiInst] = phi_reg;
    } else if (phiInst->get_type()->is_float_type()) {
        auto tmp_freg = VirtualRegister::create(Register::Float);
        for (unsigned i = 0; i < phiInst->get_num_operand(); i += 2) {
            auto *op = phiInst->get_operand(i);
            auto *bb_prev =
                static_cast<BasicBlock *>(phiInst->get_operand(i + 1));
            auto machine_bb_prev = bb_map[bb_prev];
            builder.insert_instr(
                machine_bb_prev, MachineInstr::Tag::MOV,
                {tmp_freg, context.val_map[op]},
                std::prev(machine_bb_prev->get_instrs().end()));
        }
        auto phi_freg = VirtualRegister::create(Register::Float);
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::MOV,
                          {phi_freg, tmp_freg});
        context.val_map[phiInst] = phi_freg;
    } else
        assert(false);
}

void InstructionSelector::gen_call() {

    
    // auto *callInst = static_cast<CallInst *>(context.IR_inst);
    // auto *callee = callInst->get_operand(0);
    // auto *func = static_cast<Function *>(callee);

    // unsigned int garg_cnt = 0;
    // unsigned int farg_cnt = 0;
    // unsigned int stk_cnt = 0;
    // unsigned int farg_all = 0;
    // unsigned int garg_all = 0;

    // for (auto &arg : func->get_args()) {
    //     if (arg.get_type()->is_float_type())
    //         farg_all++;
    //     else
    //         garg_all++;
    // }
    // int offset = 0;
    // offset += (garg_all > 8 ? garg_all - 8 : 0) * 8;
    // offset += (farg_all > 8 ? farg_all - 8 : 0) * 8;
    // offset = ALIGN(offset, 16);
    // if (IS_IMM_12(offset))
    //     append_inst("addi.d $sp, $sp, " + std::to_string(-offset));
    // else {
    //     load_large_int64(-offset, Reg::t(0));
    //     append_inst("add.d $sp, $sp, $t0");
    // }
    // for (auto &arg : func->get_args()) {
    //     auto *arg_val = callInst->get_operand(garg_cnt + farg_cnt + 1);
    //     if (arg.get_type()->is_float_type()) {
    //         if (farg_cnt < 8)
    //             load_to_freg(arg_val, FReg::fa(farg_cnt));
    //         else {
    //             load_to_freg(arg_val, FReg::ft(0));
    //             if (IS_IMM_12(8 * stk_cnt))
    //                 append_inst("fst.s",
    //                             {"$ft0", "$sp", std::to_string(8 * stk_cnt)});
    //             else {
    //                 load_large_int64(8 * stk_cnt, Reg::t(0));
    //                 append_inst("add.d $t0, $sp, $t0");
    //                 append_inst("fst.s", {"$ft0", "$t0", "0"});
    //             }

    //             stk_cnt++;
    //         }
    //         farg_cnt++;
    //     } else { // int or pointer
    //         if (garg_cnt < 8)
    //             load_to_greg(arg_val, Reg::a(garg_cnt));
    //         else {
    //             load_to_greg(arg_val, Reg::t(0));
    //             if (IS_IMM_12(8 * stk_cnt))
    //                 append_inst("st.d",
    //                             {"$t0", "$sp", std::to_string(8 * stk_cnt)});
    //             else {
    //                 load_large_int64(8 * stk_cnt, Reg::t(1));
    //                 append_inst("add.d $t1, $sp, $t1");
    //                 append_inst("st.d", {"$t0", "$t1", "0"});
    //             }
    //             stk_cnt++;
    //         }
    //         garg_cnt++;
    //     }
    // }
    // append_inst("bl", {func->get_name()});
    // if (IS_IMM_12(offset))
    //     append_inst("addi.d $sp, $sp, " + std::to_string(offset));
    // else {
    //     load_large_int64(offset, Reg::t(0));
    //     append_inst("add.d $sp, $sp, $t0");
    // }
    // if (not callInst->get_type()->is_void_type()) {
    //     if (callInst->get_type()->is_integer_type() ||
    //         callInst->get_type()->is_void_type())
    //         store_from_greg(callInst, Reg::a(0));
    //     else if (callInst->get_type()->is_float_type())
    //         store_from_freg(callInst, FReg::fa(0));
    // }
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
        if (std::make_shared<Immediate>(size)->is_imm_length(12)) {
            builder.gen_instr(context.machine_bb, MachineInstr::Tag::ADDI,
                              {tmp_reg1, PhysicalRegister::zero(),
                               std::make_shared<Immediate>(size)},
                              MachineInstr::Suffix::WORD);
        } else {
            builder.load_large_int32(context.machine_bb, size, tmp_reg1);
        }
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::MUL,
                          {tmp_reg2, context.val_map[idx], tmp_reg1},
                          MachineInstr::Suffix::WORD);
        builder.gen_instr(context.machine_bb, MachineInstr::Tag::ADD,
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
    builder.gen_instr(context.machine_bb, MachineInstr::Tag::MOVGR2FR_W,
                      {tmp_freg1, context.val_map[op]});
    builder.gen_instr(context.machine_bb, MachineInstr::Tag::FFINT_S_W,
                      {tmp_freg2, tmp_freg1});
    context.val_map[context.IR_inst] = tmp_freg2;
}

void InstructionSelector::gen_fptosi() {
    // 浮点数转向整数，注意向下取整(round to zero)
    auto *fptosiInst = static_cast<FpToSiInst *>(context.IR_inst);
    auto *op = fptosiInst->get_operand(0);
    auto tmp_freg = VirtualRegister::create(Register::Float);
    auto tmp_reg = VirtualRegister::create(Register::General);
    builder.gen_instr(context.machine_bb, MachineInstr::Tag::FTINTRZ_W_S,
                      {tmp_freg, context.val_map[op]});
    builder.gen_instr(context.machine_bb, MachineInstr::Tag::MOVFR2GR_S,
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