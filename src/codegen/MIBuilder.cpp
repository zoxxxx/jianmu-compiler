#include "Instruction.hpp"
#include "MIBuilder.hpp"
#include "MachineBasicBlock.hpp"
#include "MachineInstr.hpp"
#include "Operand.hpp"
#include "syntax_analyzer.h"
#include <cassert>
#include <cstdint>
#include <initializer_list>
#include <iterator>
#include <memory>

std::shared_ptr<MachineInstr> MIBuilder::append_instr(
    std::shared_ptr<MachineBasicBlock> mbb, MachineInstr::Tag tag,
    std::initializer_list<std::shared_ptr<Operand>> operands,
    MachineInstr::Suffix suffix) {
    assert(InstructionChecker::get_instance().check(tag, operands, suffix));
    std::shared_ptr<MachineInstr> instr =
        std::make_shared<MachineInstr>(tag, operands, suffix);
    mbb->append_instr(instr);
    return instr;
}

std::shared_ptr<MachineInstr> MIBuilder::insert_instr(
    std::shared_ptr<MachineBasicBlock> mbb, MachineInstr::Tag tag,
    std::initializer_list<std::shared_ptr<Operand>> operands,
    std::vector<std::shared_ptr<MachineInstr>>::iterator it,
    MachineInstr::Suffix suffix) {
    assert(InstructionChecker::get_instance().check(tag, operands, suffix));
    std::shared_ptr<MachineInstr> instr =
        std::make_shared<MachineInstr>(tag, operands, suffix);
    mbb->insert_instr(instr, it);
    return instr;
}

std::shared_ptr<MachineInstr> insert_instr_before_b(
    std::shared_ptr<MachineBasicBlock> mbb, MachineInstr::Tag tag,
    std::initializer_list<std::shared_ptr<Operand>> operands,
    MachineInstr::Suffix suffix = MachineInstr::Suffix::NONE) {
    assert(InstructionChecker::get_instance().check(tag, operands, suffix));
    std::shared_ptr<MachineInstr> instr =
        std::make_shared<MachineInstr>(tag, operands, suffix);
    auto it = mbb->get_instrs().rbegin();
    while (it != mbb->get_instrs().rend() &&
           ((*it)->get_tag() == MachineInstr::Tag::B ||
            (*it)->get_tag() == MachineInstr::Tag::BNEZ)) {
        ++it;
    }

    if (it != mbb->get_instrs().rend()) {
        auto insert_pos = std::next(it).base();
        mbb->insert_instr(instr, insert_pos);
    }
    return instr;
}

void MIBuilder::load_large_int32(std::shared_ptr<MachineBasicBlock> mbb,
                                 int32_t val, std::shared_ptr<Register> reg) {
    int32_t high_20 = val >> 12; // si20
    uint32_t low_12 = val & LOW_12_MASK;
    auto tmp_reg = VirtualRegister::create(Register::General);
    append_instr(mbb, MachineInstr::Tag::LU12I_W,
                 {tmp_reg, Immediate::create(high_20)});
    append_instr(mbb, MachineInstr::Tag::ORI,
                 {reg, tmp_reg, Immediate::create(low_12)});
}

void MIBuilder::load_large_int64(std::shared_ptr<MachineBasicBlock> mbb,
                                 int64_t val, std::shared_ptr<Register> reg) {
    auto low_32 = static_cast<int32_t>(val & LOW_32_MASK);

    auto tmp_reg1 = VirtualRegister::create(Register::General);
    load_large_int32(mbb, low_32, tmp_reg1);

    auto high_32 = static_cast<int32_t>(val >> 32);
    int32_t high_32_low_20 = (high_32 << 12) >> 12; // si20
    int32_t high_32_high_12 = high_32 >> 20;        // si12

    auto tmp_reg2 = VirtualRegister::create(Register::General);
    append_instr(mbb, MachineInstr::Tag::LU32I_D,
                 {tmp_reg2, Immediate::create(high_32_low_20)});
    append_instr(mbb, MachineInstr::Tag::LU52I_D,
                 {reg, tmp_reg2, Immediate::create(high_32_high_12)});
}

void MIBuilder::add_int_to_reg(std::shared_ptr<MachineBasicBlock> mbb,
                               std::shared_ptr<Register> dst,
                               std::shared_ptr<Register> src, int64_t val) {
    if (src == dst && val == 0) {
        return;
    }
    auto imm = Immediate::create(val);
    if (imm->is_imm_length(12))
        append_instr(mbb, MachineInstr::Tag::ADDI, {dst, src, imm});
    else {
        auto tmp_reg = VirtualRegister::create(Register::General);
        if (val <= INT32_MAX && val >= INT32_MIN)
            load_large_int32(mbb, val, tmp_reg);
        else
            load_large_int64(mbb, val, tmp_reg);
        append_instr(mbb, MachineInstr::Tag::ADD, {dst, src, tmp_reg});
    }
}

void MIBuilder::store_to_stack(std::shared_ptr<MachineBasicBlock> mbb,
                               std::shared_ptr<Register> reg,
                               std::shared_ptr<Register> ptr, int offset,
                               MachineInstr::Suffix suffix) {

    if (Immediate::create(offset)->is_imm_length(12)) {
        if (reg->get_type() == Register::RegisterType::General)
            append_instr(mbb, MachineInstr::Tag::ST,
                         {reg, ptr, Immediate::create(offset)}, suffix);
        else
            append_instr(mbb, MachineInstr::Tag::FST_S,
                         {reg, ptr, Immediate::create(offset)});
    } else {
        auto tmp_reg = VirtualRegister::create(Register::General);
        load_large_int32(mbb, offset, tmp_reg);
        append_instr(mbb, MachineInstr::Tag::ADD, {tmp_reg, ptr, tmp_reg},
                     MachineInstr::Suffix::DWORD);
        if (reg->get_type() == Register::RegisterType::General)
            append_instr(mbb, MachineInstr::Tag::ST,
                         {reg, tmp_reg, Immediate::create(0)}, suffix);
        else
            append_instr(mbb, MachineInstr::Tag::FST_S,
                         {reg, tmp_reg, Immediate::create(0)});
    }
}

void MIBuilder::load_from_stack(std::shared_ptr<MachineBasicBlock> mbb,
                                std::shared_ptr<Register> reg,
                                std::shared_ptr<Register> ptr, int offset,
                                MachineInstr::Suffix suffix) {
    if (Immediate::create(offset)->is_imm_length(12)) {
        if (reg->get_type() == Register::RegisterType::General)
            append_instr(mbb, MachineInstr::Tag::LD,
                         {reg, ptr, Immediate::create(offset)}, suffix);
        else
            append_instr(mbb, MachineInstr::Tag::FLD_S,
                         {reg, ptr, Immediate::create(offset)});
    } else {
        auto tmp_reg = VirtualRegister::create(Register::General);
        load_large_int32(mbb, offset, tmp_reg);
        append_instr(mbb, MachineInstr::Tag::ADD, {tmp_reg, ptr, tmp_reg},
                     MachineInstr::Suffix::DWORD);
        if (reg->get_type() == Register::RegisterType::General)
            append_instr(mbb, MachineInstr::Tag::LD,
                         {reg, tmp_reg, Immediate::create(0)}, suffix);
        else
            append_instr(mbb, MachineInstr::Tag::FLD_S,
                         {reg, tmp_reg, Immediate::create(0)});
    }
}

void MIBuilder::gen_prologue_epilogue(std::shared_ptr<MachineFunction> MF) {
    auto prologue = MF->get_prologue_block();
    auto epilogue = MF->get_epilogue_block();
    prologue->clear_instrs();
    epilogue->clear_instrs();
    auto fp_st_reg = VirtualRegister::create(Register::General, Register::kUSING_SP_AS_FRAME_REG);
    append_instr(prologue, MachineInstr::Tag::MOV, {fp_st_reg, PhysicalRegister::fp()});
    append_instr(prologue, MachineInstr::Tag::MOV, {PhysicalRegister::fp(), PhysicalRegister::sp()});
    add_int_to_reg(prologue, PhysicalRegister::sp(), PhysicalRegister::sp(), -MF->frame_scheduler->get_frame_size());
    for (auto reg : PhysicalRegister::callee_saved_regs()){
        if(reg->get_type() == Register::RegisterType::General){
            auto tmp_reg = VirtualRegister::create(Register::General);
            append_instr(prologue, MachineInstr::Tag::MOV, {tmp_reg, reg});
            append_instr(epilogue, MachineInstr::Tag::MOV, {reg, tmp_reg});
        }
        else if(reg->get_type() == Register::RegisterType::Float){
            auto tmp_reg = VirtualRegister::create(Register::Float);
            append_instr(prologue, MachineInstr::Tag::MOV, {tmp_reg, reg});
            append_instr(epilogue, MachineInstr::Tag::MOV, {reg, tmp_reg});
        }
        else if(reg->get_type() == Register::RegisterType::FloatCmp){
            auto tmp_reg = VirtualRegister::create(Register::FloatCmp);
            append_instr(prologue, MachineInstr::Tag::MOV, {tmp_reg, reg});
            append_instr(epilogue, MachineInstr::Tag::MOV, {reg, tmp_reg});
        }
    }
    append_instr(epilogue, MachineInstr::Tag::MOV, {PhysicalRegister::sp(), PhysicalRegister::fp()});
    append_instr(epilogue, MachineInstr::Tag::MOV, {PhysicalRegister::fp(), fp_st_reg});

    append_instr(prologue, MachineInstr::Tag::B, {std::make_shared<Label>(MF->get_name()+"_label_entry")});
    append_instr(epilogue, MachineInstr::Tag::JR, {PhysicalRegister::ra()});
}

InstructionChecker::InstructionChecker() {
    OperandType GENERAL = OperandType::GENERAL;
    OperandType FLOAT = OperandType::FLOAT;
    OperandType FLOATCMP = OperandType::FLOATCMP;
    OperandType REGISTER = OperandType::REGISTER;
    OperandType IMM = OperandType::IMM;
    OperandType LABEL = OperandType::LABEL;

    MachineInstr::Suffix BYTE = MachineInstr::Suffix::BYTE;
    MachineInstr::Suffix HALF = MachineInstr::Suffix::HALF;
    MachineInstr::Suffix WORD = MachineInstr::Suffix::WORD;
    MachineInstr::Suffix DWORD = MachineInstr::Suffix::DWORD;

    requirements[MachineInstr::Tag::MOV] =
        InstructionRequirement(2, {REGISTER, REGISTER});
    requirements[MachineInstr::Tag::ADD] = InstructionRequirement(
        3, {GENERAL, GENERAL, GENERAL}, 0, true, {WORD, DWORD});
    requirements[MachineInstr::Tag::ADDI] = InstructionRequirement(
        3, {GENERAL, GENERAL, IMM}, 12, true, {WORD, DWORD});
    requirements[MachineInstr::Tag::SUB] = InstructionRequirement(
        3, {GENERAL, GENERAL, GENERAL}, 0, true, {WORD, DWORD});
    requirements[MachineInstr::Tag::MUL] = InstructionRequirement(
        3, {GENERAL, GENERAL, GENERAL}, 0, true, {WORD, DWORD});
    requirements[MachineInstr::Tag::DIV] = InstructionRequirement(
        3, {GENERAL, GENERAL, GENERAL}, 0, true, {WORD, DWORD});
    requirements[MachineInstr::Tag::MOD] = InstructionRequirement(
        3, {GENERAL, GENERAL, GENERAL}, 0, true, {WORD, DWORD});
    requirements[MachineInstr::Tag::LU12I_W] =
        InstructionRequirement(2, {GENERAL, IMM}, 20, true);
    requirements[MachineInstr::Tag::LU32I_D] =
        InstructionRequirement(2, {GENERAL, IMM}, 20, true);
    requirements[MachineInstr::Tag::LU52I_D] =
        InstructionRequirement(3, {GENERAL, GENERAL, IMM}, 12, true);
    requirements[MachineInstr::Tag::SLT] =
        InstructionRequirement(3, {GENERAL, GENERAL, GENERAL});
    requirements[MachineInstr::Tag::SLTU] =
        InstructionRequirement(3, {GENERAL, GENERAL, GENERAL});
    requirements[MachineInstr::Tag::SLTI] =
        InstructionRequirement(3, {GENERAL, GENERAL, IMM}, 12, true);
    requirements[MachineInstr::Tag::SLTUI] =
        InstructionRequirement(3, {GENERAL, GENERAL, IMM}, 12, true);
    requirements[MachineInstr::Tag::AND] =
        InstructionRequirement(3, {GENERAL, GENERAL, GENERAL});
    requirements[MachineInstr::Tag::OR] =
        InstructionRequirement(3, {GENERAL, GENERAL, GENERAL});
    requirements[MachineInstr::Tag::NOR] =
        InstructionRequirement(3, {GENERAL, GENERAL, GENERAL});
    requirements[MachineInstr::Tag::XOR] =
        InstructionRequirement(3, {GENERAL, GENERAL, GENERAL});
    requirements[MachineInstr::Tag::ANDN] =
        InstructionRequirement(3, {GENERAL, GENERAL, GENERAL});
    requirements[MachineInstr::Tag::ORN] =
        InstructionRequirement(3, {GENERAL, GENERAL, GENERAL});
    requirements[MachineInstr::Tag::ANDI] =
        InstructionRequirement(3, {GENERAL, GENERAL, IMM}, 12, false);
    requirements[MachineInstr::Tag::ORI] =
        InstructionRequirement(3, {GENERAL, GENERAL, IMM}, 12, false);
    requirements[MachineInstr::Tag::XORI] =
        InstructionRequirement(3, {GENERAL, GENERAL, IMM}, 12, false);
    requirements[MachineInstr::Tag::BEQ] =
        InstructionRequirement(3, {GENERAL, GENERAL, LABEL});
    requirements[MachineInstr::Tag::BNE] =
        InstructionRequirement(3, {GENERAL, GENERAL, LABEL});
    requirements[MachineInstr::Tag::BLT] =
        InstructionRequirement(3, {GENERAL, GENERAL, LABEL});
    requirements[MachineInstr::Tag::BGE] =
        InstructionRequirement(3, {GENERAL, GENERAL, LABEL});
    requirements[MachineInstr::Tag::BLTU] =
        InstructionRequirement(3, {GENERAL, GENERAL, LABEL});
    requirements[MachineInstr::Tag::BGEU] =
        InstructionRequirement(3, {GENERAL, GENERAL, LABEL});
    requirements[MachineInstr::Tag::BEQZ] =
        InstructionRequirement(2, {GENERAL, LABEL});
    requirements[MachineInstr::Tag::BNEZ] =
        InstructionRequirement(2, {GENERAL, LABEL});
    requirements[MachineInstr::Tag::B] = InstructionRequirement(1, {LABEL});
    requirements[MachineInstr::Tag::BL] = InstructionRequirement(1, {LABEL});
    requirements[MachineInstr::Tag::JIRL] =
        InstructionRequirement(3, {GENERAL, GENERAL, LABEL});
    requirements[MachineInstr::Tag::LD] = InstructionRequirement(
        3, {GENERAL, GENERAL, IMM}, 12, true, {BYTE, HALF, WORD, DWORD});
    requirements[MachineInstr::Tag::ST] = InstructionRequirement(
        3, {GENERAL, GENERAL, IMM}, 12, true, {BYTE, HALF, WORD, DWORD});
    requirements[MachineInstr::Tag::JR] = InstructionRequirement(1, {GENERAL});
    requirements[MachineInstr::Tag::LA_LOCAL] =
        InstructionRequirement(2, {GENERAL, LABEL});
    requirements[MachineInstr::Tag::FADD_S] =
        InstructionRequirement(3, {FLOAT, FLOAT, FLOAT});
    requirements[MachineInstr::Tag::FSUB_S] =
        InstructionRequirement(3, {FLOAT, FLOAT, FLOAT});
    requirements[MachineInstr::Tag::FMUL_S] =
        InstructionRequirement(3, {FLOAT, FLOAT, FLOAT});
    requirements[MachineInstr::Tag::FDIV_S] =
        InstructionRequirement(3, {FLOAT, FLOAT, FLOAT});
    requirements[MachineInstr::Tag::FFINT_S_W] =
        InstructionRequirement(2, {FLOAT, FLOAT});
    requirements[MachineInstr::Tag::FTINTRZ_W_S] =
        InstructionRequirement(2, {FLOAT, FLOAT});
    requirements[MachineInstr::Tag::MOVGR2FR_W] =
        InstructionRequirement(2, {FLOAT, GENERAL});
    requirements[MachineInstr::Tag::MOVFR2GR_S] =
        InstructionRequirement(2, {GENERAL, FLOAT});
    requirements[MachineInstr::Tag::MOVCF2GR] =
        InstructionRequirement(2, {GENERAL, FLOATCMP});
    requirements[MachineInstr::Tag::MOVGR2CF] =
        InstructionRequirement(2, {FLOATCMP, GENERAL});
    requirements[MachineInstr::Tag::FLD_S] =
        InstructionRequirement(3, {FLOAT, GENERAL, IMM}, 12, true);
    requirements[MachineInstr::Tag::FST_S] =
        InstructionRequirement(3, {FLOAT, GENERAL, IMM}, 12, true);
    requirements[MachineInstr::Tag::FCMP_SEQ_S] =
        InstructionRequirement(3, {FLOATCMP, FLOAT, FLOAT});
    requirements[MachineInstr::Tag::FCMP_SNE_S] =
        InstructionRequirement(3, {FLOATCMP, FLOAT, FLOAT});
    requirements[MachineInstr::Tag::FCMP_SLT_S] =
        InstructionRequirement(3, {FLOATCMP, FLOAT, FLOAT});
    requirements[MachineInstr::Tag::FCMP_SLE_S] =
        InstructionRequirement(3, {FLOATCMP, FLOAT, FLOAT});
    requirements[MachineInstr::Tag::BCEQZ] =
        InstructionRequirement(2, {FLOATCMP, LABEL});
    requirements[MachineInstr::Tag::BCNEZ] =
        InstructionRequirement(2, {FLOATCMP, LABEL});
}

bool InstructionChecker::check_operands_type(
    MachineInstr::Tag tag, std::vector<std::shared_ptr<Operand>> operands) {
    auto requirement = requirements[tag];
    if (operands.size() != requirement.num_operands) {
        assert(false && "operand number not match");
        return false;
    }
    for (size_t i = 0; i < operands.size(); i++) {
        if (std::dynamic_pointer_cast<Register>(operands[i]) != nullptr) {
            if (requirement.operand_types[i] == OperandType::REGISTER)
                return true;
            auto reg = std::dynamic_pointer_cast<Register>(operands[i]);
            if (reg->get_type() == Register::RegisterType::General &&
                requirement.operand_types[i] != OperandType::GENERAL) {
                assert(false && "operand type not match");
                return false;
            }
            if (reg->get_type() == Register::RegisterType::Float &&
                requirement.operand_types[i] != OperandType::FLOAT) {
                assert(false && "operand type not match");
                return false;
            }
            if (reg->get_type() == Register::RegisterType::FloatCmp &&
                requirement.operand_types[i] != OperandType::FLOATCMP) {
                assert(false && "operand type not match");
                return false;
            }
        }
        if (std::dynamic_pointer_cast<Immediate>(operands[i]) != nullptr &&
            requirement.operand_types[i] != OperandType::IMM) {
            assert(false && "operand type not match");
            return false;
        }
        if (std::dynamic_pointer_cast<Label>(operands[i]) != nullptr &&
            requirement.operand_types[i] != OperandType::LABEL) {
            assert(false && "operand type not match");
            return false;
        }
    }
    return true;
}

bool InstructionChecker::check_suffix(MachineInstr::Tag tag,
                                      MachineInstr::Suffix suffix) {
    auto requirement = requirements[tag];
    for (auto s : requirement.suffixes) {
        if (s == suffix) {
            return true;
        }
    }
    assert(false && "suffix not match");
    return false;
}

bool InstructionChecker::check_imm(
    MachineInstr::Tag tag, std::vector<std::shared_ptr<Operand>> operands,
    size_t imm_length, bool imm_signed) {
    auto requirement = requirements[tag];
    for (auto operand : operands) {
        if (operand->is_imm()) {
            auto imm = std::dynamic_pointer_cast<Immediate>(operand);
            if (imm_signed && !imm->is_imm_length(imm_length)) {
                assert(false && "imm length not match");
                return false;
            }
            if (!imm_signed && !imm->is_uimm_length(imm_length)) {
                assert(false && "imm length not match");
                return false;
            }
        }
    }
    return true;
}

bool InstructionChecker::check_other(
    MachineInstr::Tag tag, std::vector<std::shared_ptr<Operand>> operands) {
    if (tag == MachineInstr::Tag::MOV) {
        if (std::dynamic_pointer_cast<Register>(operands[0])->get_type() !=
            std::dynamic_pointer_cast<Register>(operands[1])->get_type()) {
            assert(false && "mov type not match");
            return false;
        }
    }
    return true;
}