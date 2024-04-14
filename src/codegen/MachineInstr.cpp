#include "MachineBasicBlock.hpp"
#include "MachineFunction.hpp"
#include "MachineInstr.hpp"
#include "Operand.hpp"
#include "ast.hpp"
#include <cstddef>
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
    case MachineInstr::Tag::SLL:
        return "sll";
    case MachineInstr::Tag::SRL:
        return "srl";
    case MachineInstr::Tag::SLLI:
        return "slli";
    case MachineInstr::Tag::SRLI:
        return "srli";
    case MachineInstr::Tag::SRAI:
        return "srai";
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
        return "movgr2fr.w";
    case MachineInstr::Tag::MOVFR2GR_S:
        return "movfr2gr.s";
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
    assert(false && "unknown tag");
    return "";
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

std::string MachineInstr::print_mov() const {
    if (operands[0] == operands[1]) {
        return "";
    }
    if (operands[0] == PhysicalRegister::zero()) {
        return "";
    }
    auto type = std::dynamic_pointer_cast<Register>(operands[0])->get_type();
    if (type == Register::RegisterType::Float) {
        return "fmov.s " + operands[0]->get_name() + ", " +
               operands[1]->get_name() + "\n";
    }
    if (type == Register::RegisterType::General) {
        return "ori " + operands[0]->get_name() + ", " +
               operands[1]->get_name() + ", 0\n";
    }
    if (type == Register::RegisterType::FloatCmp) {
        assert(false && "FloatCmp register should not be moved");
        return "";
    }
    return "";
}
std::string MachineInstr::print() const {
    if (tag == Tag::MOV)
        return print_mov();
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
        res += operand->get_name();
    }
    if (comment != "") {
        res += " # " + comment;
    }
    res += "\n";
    return res;
}

bool MachineInstr::has_dst() const {
    return !(tag == Tag::BEQ || tag == Tag::BNE || tag == Tag::BLT ||
             tag == Tag::BGE || tag == Tag::BLTU || tag == Tag::BGEU ||
             tag == Tag::BEQZ || tag == Tag::BNEZ || tag == Tag::B ||
             tag == Tag::BL || tag == Tag::BCEQZ || tag == Tag::BCNEZ ||
             tag == Tag::JR || tag == Tag::ST || tag == Tag::FST_S);
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

std::vector<std::shared_ptr<Register>> MachineInstr::get_use() {
    std::vector<std::shared_ptr<Register>> uses;
    if (tag == Tag::BL) {
        auto mf = std::dynamic_pointer_cast<Label>(operands[0])
                      ->get_function()
                      .lock();
        for (auto &arg : mf->get_IR_function()->get_args()) {
            if (!mf->params_schedule_map[&arg].on_stack) {
                uses.push_back(std::dynamic_pointer_cast<Register>(
                    mf->params_schedule_map[&arg].reg));
            }
        }
        return uses;
    }
    if (tag == Tag::JR) {
        for (auto reg : PhysicalRegister::callee_saved_regs()) {
            uses.push_back(reg);
        }
        auto type = this->get_parent()
                        .lock() // machine basic block
                        ->get_parent()
                        .lock()             // machine function
                        ->get_IR_function() // ir function
                        ->get_return_type();
        if (type->is_integer_type()) {
            uses.push_back(PhysicalRegister::a(0));
        } else if (type->is_float_type()) {
            uses.push_back(PhysicalRegister::fa(0));
        }
        return uses;
    }
    for (auto &operand : operands) {
        if (has_dst() && operand == operands[0])
            continue;
        if (operand == PhysicalRegister::zero())
            continue;
        if (std::dynamic_pointer_cast<Register>(operand) != nullptr) {
            uses.push_back(std::dynamic_pointer_cast<Register>(operand));
        }
    }
    return uses;
}

std::vector<std::shared_ptr<Register>> MachineInstr::get_def() {
    std::vector<std::shared_ptr<Register>> defs;
    if (tag == Tag::BL) {
        for (auto reg : PhysicalRegister::caller_saved_regs()) {
            defs.push_back(reg);
        }
        defs.push_back(PhysicalRegister::ra());
        return defs;
    }
    if (has_dst() && get_dst() != PhysicalRegister::zero()) {
        defs.push_back(get_dst());
    }
    return defs;
}

void MachineInstr::replace_def(std::shared_ptr<Register> old_reg,
                               std::shared_ptr<Register> new_reg) {
    if (has_dst() && get_dst() == old_reg) {
        operands[0] = new_reg;
    }
}

void MachineInstr::replace_use(std::shared_ptr<Register> old_reg,
                               std::shared_ptr<Register> new_reg) {
    size_t begin = has_dst() ? 1 : 0;
    for (size_t i = begin; i < operands.size(); i++) {
        if (operands[i] == old_reg) {
            operands[i] = new_reg;
        }
    }
}

void MachineInstr::colorize() {
    for (auto &operand : operands) {
        if (std::dynamic_pointer_cast<VirtualRegister>(operand) == nullptr)
            continue;
        auto vr = std::dynamic_pointer_cast<VirtualRegister>(operand);
        auto it = Register::color.find(vr);
        if (it != Register::color.end() && it->second != nullptr) {
            operand = it->second;
        } else {
            assert(false && "Virtual register not colored");
        }
    }
}
