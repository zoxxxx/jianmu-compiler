#pragma once
#include "BasicBlock.hpp"
#include "MachineBasicBlock.hpp"
#include "Operand.hpp"
#include <locale>
#include <memory>
#include <string>
#include <vector>

class MachineBasicBlock;
class Operand;
class MachineInstr : public std::enable_shared_from_this<MachineInstr> {
  public:
    enum class Suffix { BYTE, HALF, WORD, DWORD, NONE };
    enum class Tag {
        // move operation, though it is not a real instruction
        MOV,
        // arithmetical operation
        ADD,
        ADDI,
        SUB,
        MUL,
        DIV,
        MOD,
        // logical operation
        LU12I_W,
        LU32I_D,
        LU52I_D,
        // compare operation
        SLT,
        SLTU,
        SLTI,
        SLTUI,
        // bitwise operation
        AND,
        OR,
        NOR,
        XOR,
        ANDN,
        ORN,
        ANDI,
        ORI,
        XORI,
        // branch operation
        BEQ,
        BNE,
        BLT,
        BGE,
        BLTU,
        BGEU,
        BEQZ,
        BNEZ,
        B,
        BL,
        JIRL,
        // load and store operation
        LD,
        ST,
        // macro operation
        JR,
        // float arithmetical operation
        FADD_S,
        FSUB_S,
        FMUL_S,
        FDIV_S,
        // float convert operation
        FFINT_S_W,
        FTINTRZ_W_S,
        // float move operation
        MOVGR2FR_S,
        MOVFR2GR_W,
        // float load and store operation
        FLD_S,
        FST_S,
        // float compare operation
        FCMP_SEQ_S,
        FCMP_SNE_S,
        FCMP_SLT_S,
        FCMP_SLE_S,
        // float branch operation
        BCEQZ,
        BCNEZ
    };

    
    bool has_dst() const;
    std::shared_ptr<Operand> get_operand(unsigned index) const; 
    std::shared_ptr<Operand> get_dst() const ;

    std::weak_ptr<MachineBasicBlock> get_parent() const { return parent; }
    void set_comment(const std::string &comment) { this->comment = comment; }

    Tag get_tag() const { return tag; }
    Suffix get_suffix() const { return suffix; }

    std::string print() const;

    MachineInstr(std::weak_ptr<MachineBasicBlock> parent, Tag tag,
                 std::vector<std::shared_ptr<Operand>> operands,
                 Suffix suffix = Suffix::NONE)
        : parent(parent), suffix(suffix), tag(tag), operands(operands) {}

  private:
    std::shared_ptr<MachineBasicBlock> parent;
    std::string comment;

    Suffix suffix;
    Tag tag;

    std::vector<std::shared_ptr<Operand>> operands;
};