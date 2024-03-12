#include "Instruction.hpp"
#include "MIbuilder.hpp"
#include "MachineBasicBlock.hpp"
#include "MachineInstr.hpp"
#include "Operand.hpp"
#include "syntax_analyzer.h"
#include <cassert>
#include <memory>

std::shared_ptr<MachineInstr>
MIBuilder::gen_instr(std::shared_ptr<MachineBasicBlock> mbb,
                     MachineInstr::Tag tag,
                     std::vector<std::shared_ptr<Operand>> operands,
                     MachineInstr::Suffix suffix) {
    assert(InstructionChecker::get_instance().check(tag, operands, suffix));
    std::shared_ptr<MachineInstr> instr =
        std::make_shared<MachineInstr>(tag, operands, suffix);
    mbb->append_instr(instr);
    return instr;
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
    requirements[MachineInstr::Tag::MOVGR2FR_S] =
        InstructionRequirement(2, {FLOAT, GENERAL});
    requirements[MachineInstr::Tag::MOVFR2GR_W] =
        InstructionRequirement(2, {GENERAL, FLOAT});
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