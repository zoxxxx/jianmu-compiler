#pragma once
#include "BasicBlock.hpp"
#include "MachineBasicBlock.hpp"
#include "Operand.hpp"
#include <locale>
#include <memory>
#include <string>
#include <vector>

class Operand;
class Register;
class MachineBasicBlock;
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
        // shift operation
        SLL,
        SRL,
        SLLI,
        SRLI,
        SRAI,
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
        LA_LOCAL,
        // float arithmetical operation
        FADD_S,
        FSUB_S,
        FMUL_S,
        FDIV_S,
        // float convert operation
        FFINT_S_W,
        FTINTRZ_W_S,
        // float move operation
        MOVGR2FR_W,
        MOVFR2GR_S,
        // floatcmp move operation
        MOVGR2CF,
        MOVCF2GR,
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
    enum Flag {
        IS_FUNC_ARGS_SET = 1<<0,
        IS_FRAME_SET = 1<<1,
        IS_PHI_MOV = 1<<2,
        IS_RESERVED = 1<<3
    };
    bool is_func_args_set() const {return flag & Flag::IS_FUNC_ARGS_SET; }
    bool is_frame_set() const { return flag & Flag::IS_FRAME_SET; }
    bool is_phi_mov() const { return flag & Flag::IS_PHI_MOV;}
    bool is_reserved() const { return flag & Flag::IS_RESERVED; }

    bool has_dst() const;
    std::shared_ptr<Operand> get_operand(unsigned index) const; 
    std::shared_ptr<Register> get_dst() const ;

    std::weak_ptr<MachineBasicBlock> get_parent() const { return parent; }
    void set_comment(const std::string &comment) { this->comment = comment; }

    Tag get_tag() const { return tag; }
    Suffix get_suffix() const { return suffix; }

    std::string print() const;
    std::string print_mov() const;

    MachineInstr(std::weak_ptr<MachineBasicBlock> parent, Tag tag,
                 std::vector<std::shared_ptr<Operand>> operands,
                 Suffix suffix, unsigned flag)
        : flag(flag), parent(parent), suffix(suffix), tag(tag), operands(operands) {}
    
    std::vector<std::shared_ptr<Register>> get_use() ;
    std::vector<std::shared_ptr<Register>> get_def() ;
    void replace_def(std::shared_ptr<Register> old_reg, std::shared_ptr<Register> new_reg);
    void replace_use(std::shared_ptr<Register> old_reg, std::shared_ptr<Register> new_reg);

    void colorize();
  private:
    unsigned flag;
    std::shared_ptr<MachineBasicBlock> parent;
    std::string comment;

    Suffix suffix;
    Tag tag;

    std::vector<std::shared_ptr<Operand>> operands;
};