#include "MachineBasicBlock.hpp"
#include "MachineInstr.hpp"
#include "Operand.hpp"
#include "ast.hpp"
#include <memory>

std::string get_tag_name(MachineInstr::Tag tag) {
    switch (tag) {
    case MachineInstr::Tag::MOV:
        return "mov";
    case MachineInstr::Tag::ADD:
        return "add";
    case MachineInstr::Tag::ADDI:
        return "addi";
    case MachineInstr::Tag::SUB:
        return "sub";
    case MachineInstr::Tag::MUL:
        return "mul";
    case MachineInstr::Tag::DIV:
        return "div";
    case MachineInstr::Tag::MOD:
        return "mod";
    case MachineInstr::Tag::LU12I_W:
        return "lu12i.w";
    case MachineInstr::Tag::LU32I_D:
        return "lu32i.d";
    case MachineInstr::Tag::LU52I_D:
        return "lu52i.d";
    case MachineInstr::Tag::SLT:
        return "slt";
    case MachineInstr::Tag::SLTU:
        return "sltu";
    case MachineInstr::Tag::SLTI:
        return "slti";
    case MachineInstr::Tag::SLTUI:
        return "sltui";
    case MachineInstr::Tag::AND:
        return "and";
    case MachineInstr::Tag::OR:
        return "or";
    case MachineInstr::Tag::NOR:
        return "nor";
    case MachineInstr::Tag::XOR:
        return "xor";
    case MachineInstr::Tag::ANDN:
        return "andn";
    case MachineInstr::Tag::ORN:
        return "orn";
    case MachineInstr::Tag::ANDI:
        return "andi";
    case MachineInstr::Tag::ORI:
        return "ori";
    case MachineInstr::Tag::XORI:
        return "xori";
    case MachineInstr::Tag::BEQ:
        return "beq";
    case MachineInstr::Tag::BNE:
        return "bne";
    case MachineInstr::Tag::BLT:
        return "blt";
    case MachineInstr::Tag::BGE:
        return "bge";
    case MachineInstr::Tag::BLTU:
        return "bltu";
    case MachineInstr::Tag::BGEU:
        return "bgeu";
    case MachineInstr::Tag::BEQZ:
        return "beqz";
    case MachineInstr::Tag::BNEZ:
        return "bnez";
    case MachineInstr::Tag::B:
        return "b";
    case MachineInstr::Tag::BL:
        return "bl";
    case MachineInstr::Tag::JIRL:
        return "jirl";
    case MachineInstr::Tag::LD:
        return "ld";
    case MachineInstr::Tag::ST:
        return "st";
    case MachineInstr::Tag::JR:
        return "jr";
    case MachineInstr::Tag::LA_LOCAL:
        return "la.local";
    case MachineInstr::Tag::FADD_S:
        return "fadd.s";
    case MachineInstr::Tag::FSUB_S:
        return "fsub.s";
    case MachineInstr::Tag::FMUL_S:
        return "fmul.s";
    case MachineInstr::Tag::FDIV_S:
        return "fdiv.s";
    case MachineInstr::Tag::FFINT_S_W:
        return "ffint.s.w";
    case MachineInstr::Tag::FTINTRZ_W_S:
        return "ftintrz.w.s";
    case MachineInstr::Tag::MOVGR2FR_W:
        return "movgr2fr.s";
    case MachineInstr::Tag::MOVFR2GR_S:
        return "movfr2gr.w";
    case MachineInstr::Tag::MOVGR2CF:
        return "movgr2cf";
    case MachineInstr::Tag::MOVCF2GR:
        return "movcf2gr";
    case MachineInstr::Tag::FLD_S:
        return "fld.s";
    case MachineInstr::Tag::FST_S:
        return "fst.s";
    case MachineInstr::Tag::FCMP_SEQ_S:
        return "fcmp.seq.s";
    case MachineInstr::Tag::FCMP_SNE_S:
        return "fcmp.sne.s";
    case MachineInstr::Tag::FCMP_SLT_S:
        return "fcmp.slt.s";
    case MachineInstr::Tag::FCMP_SLE_S:
        return "fcmp.sle.s";
    case MachineInstr::Tag::BCEQZ:
        return "bceqz";
    case MachineInstr::Tag::BCNEZ:
        return "bcnez";
    }
}

std::string get_suffix_name(MachineInstr::Suffix suffix) {
    switch (suffix) {
    case MachineInstr::Suffix::BYTE:
        return ".b";
    case MachineInstr::Suffix::HALF:
        return ".h";
    case MachineInstr::Suffix::WORD:
        return ".w";
    case MachineInstr::Suffix::DWORD:
        return ".d";
    case MachineInstr::Suffix::NONE:
        return "";
    default:
        assert(false && "unknown suffix");
    }
}

std::string MachineInstr::print() const {
    std::string res = get_tag_name(tag);
    res += get_suffix_name(suffix);
    res += " ";
    bool first = true;
    for (auto &operand : operands) {
        if (first) {
            first = false;
        } else {
            res += ", ";
        }
        res += operand->print();
    }
    if (comment != "") {
        res += " # " + comment;
    }
    return res;
}

bool MachineInstr::has_dst() const {
    return !(tag == Tag::BEQ || tag == Tag::BNE || tag == Tag::BLT ||
             tag == Tag::BGE || tag == Tag::BLTU || tag == Tag::BGEU ||
             tag == Tag::BEQZ || tag == Tag::BNEZ || tag == Tag::B ||
             tag == Tag::BL || tag == Tag::BCEQZ || tag == Tag::BCNEZ ||
             tag == Tag::JR);
}

std::shared_ptr<Operand> MachineInstr::get_operand(unsigned index) const {
    assert(index < operands.size() && "Index out of range!");
    return operands[index];
}

std::shared_ptr<Register> MachineInstr::get_dst() const {
    assert(has_dst() && "Instruction has no destination operand!");
    assert(std::dynamic_pointer_cast<Register>(operands[0]) != nullptr &&
           "Destination operand is not a register!");
    return std::dynamic_pointer_cast<Register>(operands[0]);
}