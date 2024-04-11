#include "MachineFunction.hpp"
#include "MachineInstr.hpp"
#include "Operand.hpp"
#include "PeepholeOptimization.hpp"
#include <memory>

bool PeepholeOptimization::check_const(std::shared_ptr<MachineInstr> inst,
                                       int &imm) {
    if ((inst->get_tag() == MachineInstr::Tag::ADDI ||
         inst->get_tag() == MachineInstr::Tag::ORI) &&
        inst->get_operand(1) == PhysicalRegister::zero()) {
        imm = std::dynamic_pointer_cast<Immediate>(inst->get_operand(2))
                  ->get_value();
        return true;
    }
    return false;
}

template <typename T>
inst_it PeepholeOptimization::find_pre_inst(inst_it inst, T pred) {
    if (inst == mbb->get_instrs_begin())
        return mbb->get_instrs_end();
    for (auto inst2_ = std::prev(inst); inst2_ != mbb->get_instrs_begin();
         inst2_--) {
        auto inst2 = *inst2_;
        if (pred(inst2)) {
            return inst2_;
        }
    }
    return mbb->get_instrs_end();
}

void PeepholeOptimization::opt_mul(inst_it &inst_) {
    auto inst = *inst_;
    if (inst->get_tag() != MachineInstr::Tag::MUL)
        return;
    auto inst2_ =
        find_pre_inst(inst_, [inst](std::shared_ptr<MachineInstr> inst_x) {
            return inst_x->has_dst() &&
                   inst_x->get_dst() == inst->get_operand(1);
        });
    auto inst3_ =
        find_pre_inst(inst_, [inst](std::shared_ptr<MachineInstr> inst_x) {
            return inst_x->has_dst() &&
                   inst_x->get_dst() == inst->get_operand(2);
        });

    int imm = 0;
    if (inst2_ == inst3_) {
        if (inst2_ == mbb->get_instrs_end())
            return;
        auto inst2 = *inst2_;
        if (check_const(inst2, imm)) {
            inst_ = mbb->erase_instr(inst_);
            builder->set_insert_point(mbb, inst_);
            builder->load_int32(imm * imm, inst->get_dst());
        }
    } else if (inst2_ != mbb->get_instrs_end() && check_const(*inst2_, imm) &&
               (imm & (imm - 1)) == 0) {
        if (imm == 0) {
            inst_ = mbb->erase_instr(inst_);
            builder->set_insert_point(mbb, inst_);
            builder->load_int32(0, inst->get_dst());
            return;
        }
        auto inst2 = *inst2_;
        int bits = __builtin_ffs(imm) - 1;
        inst_ = mbb->erase_instr(inst_);
        builder->set_insert_point(mbb, inst_);
        builder->insert_instr(
            MachineInstr::Tag::SLLI,
            {inst->get_dst(), inst->get_operand(2), Immediate::create(bits)},
            inst->get_suffix());
    } else if (inst3_ != mbb->get_instrs_end() && check_const(*inst3_, imm) &&
               (imm & (imm - 1)) == 0) {
        if (imm == 0) {
            inst_ = mbb->erase_instr(inst_);
            builder->set_insert_point(mbb, inst_);
            builder->load_int32(0, inst->get_dst());
            return;
        }
        auto inst3 = *inst3_;
        int bits = __builtin_ffs(imm) - 1;
        inst_ = mbb->erase_instr(inst_);
        builder->set_insert_point(mbb, inst_);
        builder->insert_instr(
            MachineInstr::Tag::SLLI,
            {inst->get_dst(), inst->get_operand(1), Immediate::create(bits)},
            inst->get_suffix());
    }
}

void PeepholeOptimization::opt_div(inst_it &inst_) {
    auto inst = *inst_;
    if (inst->get_tag() != MachineInstr::Tag::DIV)
        return;
    auto inst2_ =
        find_pre_inst(inst_, [inst](std::shared_ptr<MachineInstr> inst_x) {
            return inst_x->has_dst() &&
                   inst_x->get_dst() == inst->get_operand(2);
        });
    if (inst2_ == mbb->get_instrs_end())
        return;
    auto inst2 = *inst2_;

    int imm = 0;
    if (check_const(inst2, imm) && (imm & (imm - 1)) == 0) {
        int bits = __builtin_ffs(imm) - 1;
        inst_ = mbb->erase_instr(inst_);
        builder->set_insert_point(mbb, inst_);
        builder->insert_instr(
            MachineInstr::Tag::SRAI,
            {inst->get_dst(), inst->get_operand(1), Immediate::create(bits)},
            inst->get_suffix());
    }
}

void PeepholeOptimization::opt_add(inst_it &inst_) {
    auto inst = *inst_;
    if (inst->get_tag() != MachineInstr::Tag::ADD)
        return;
    auto inst2_ =
        find_pre_inst(inst_, [inst](std::shared_ptr<MachineInstr> inst_x) {
            return inst_x->has_dst() &&
                   inst_x->get_dst() == inst->get_operand(1);
        });
    auto inst3_ =
        find_pre_inst(inst_, [inst](std::shared_ptr<MachineInstr> inst_x) {
            return inst_x->has_dst() &&
                   inst_x->get_dst() == inst->get_operand(2);
        });

    int imm = 0;
    if (inst2_ == inst3_) {
        if (inst2_ == mbb->get_instrs_end())
            return;
        auto inst2 = *inst2_;
        if (check_const(inst2, imm)) {
            inst_ = mbb->erase_instr(inst_);
            builder->set_insert_point(mbb, inst_);
            builder->load_int32(2 * imm, inst->get_dst());
        }
    } else if (inst2_ != mbb->get_instrs_end() && check_const(*inst2_, imm)) {
        auto inst2 = *inst2_;
        inst_ = mbb->erase_instr(inst_);
        builder->set_insert_point(mbb, inst_);
        builder->insert_instr(
            MachineInstr::Tag::ADDI,
            {inst->get_dst(), inst->get_operand(2), Immediate::create(imm)},
            inst->get_suffix());
    } else if (inst3_ != mbb->get_instrs_end() && check_const(*inst3_, imm)) {
        auto inst3 = *inst3_;
        inst_ = mbb->erase_instr(inst_);
        builder->set_insert_point(mbb, inst_);
        builder->add_int_to_reg(
            std::dynamic_pointer_cast<Register>(inst->get_dst()),
            std::dynamic_pointer_cast<Register>(inst->get_operand(1)), imm);
    }
}

void PeepholeOptimization::run_on_func(std::shared_ptr<MachineFunction> func) {
#ifndef MAX_ITER
#define MAX_ITER 100000
#endif
    for (auto &mbb_ : func->get_basic_blocks()) {
        mbb = mbb_;
        int cnt = 0;
        while (true && ++cnt < MAX_ITER) {
            bool changed = false;
            for (auto inst = mbb->get_instrs_begin();
                 inst != mbb->get_instrs_end(); inst++) {
                opt_add(inst);
                opt_mul(inst);
                opt_div(inst);
            }
            if (!changed) {
                break;
            }
        }
    }
#undef MAX_ITER
}

void PeepholeOptimization::run() {
    for (auto &func : module->get_functions()) {
        run_on_func(func);
    }
}