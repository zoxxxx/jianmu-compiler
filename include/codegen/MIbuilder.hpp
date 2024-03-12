#include "Instruction.hpp"
#include "MachineBasicBlock.hpp"
#include "MachineInstr.hpp"
#include "Operand.hpp"
#include "Type.hpp"
#include <cassert>
#include <memory>
#include <typeindex>
#include <unordered_map>
class InstructionChecker {
  public:
    enum class OperandType { GENERAL, FLOAT, FLOATCMP, REGISTER, IMM, LABEL };
    struct InstructionRequirement {
        size_t num_operands;                    // 需要的操作数数量
        std::vector<OperandType> operand_types; // 每个操作数的类型
        size_t imm_length; // 立即数的最大长度，如果有的话
        bool imm_signed;   // 立即数是否有符号
        std::vector<MachineInstr::Suffix> suffixes;

        InstructionRequirement(
            size_t num_operands,
            std::initializer_list<OperandType> operands_types,
            size_t imm_length = 0, bool imm_signed = true,
            std::vector<MachineInstr::Suffix> suffixes =
                {MachineInstr::Suffix::NONE})
            : num_operands(num_operands), operand_types(operands_types),
              imm_length(imm_length), imm_signed(imm_signed),
              suffixes(suffixes) {}
    };
    static InstructionChecker &get_instance() {
        static InstructionChecker instance;
        return instance;
    }
    bool check(MachineInstr::Tag tag,
               std::vector<std::shared_ptr<Operand>> operands,
               MachineInstr::Suffix suffix);

  private:
    bool check_operands_type(MachineInstr::Tag tag,
                             std::vector<std::shared_ptr<Operand>> operands);
    bool check_suffix(MachineInstr::Tag tag, MachineInstr::Suffix suffix);
    bool check_imm(MachineInstr::Tag tag,
                   std::vector<std::shared_ptr<Operand>> operands,
                   size_t imm_length, bool imm_signed);
    bool check_other(MachineInstr::Tag tag,
                     std::vector<std::shared_ptr<Operand>> operands);
    std::unordered_map<MachineInstr::Tag, InstructionRequirement> requirements;
    InstructionChecker();
};

class MIBuilder {
  private:
  public:
    std::shared_ptr<MachineInstr>
    gen_instr(std::shared_ptr<MachineBasicBlock>, MachineInstr::Tag tag,
              std::vector<std::shared_ptr<Operand>> operands,
              MachineInstr::Suffix suffix = MachineInstr::Suffix::NONE);
};
