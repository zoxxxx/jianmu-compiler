#pragma once

#include "BasicBlock.hpp"
#include "MachineFunction.hpp"
#include "MachineInstr.hpp"
#include "MachineModule.hpp"
#include "Operand.hpp"

#include <memory>
#include <vector>

using RegisterSet = std::set<std::shared_ptr<Register>,
                             std::owner_less<std::shared_ptr<Register>>>;
using MBBSetMap = std::map<std::shared_ptr<MachineBasicBlock>, RegisterSet,
                           std::owner_less<std::shared_ptr<MachineBasicBlock>>>;

class Operand;
class MachineInstr;
class MachineFunction;
class MachineBasicBlock
    : public std::enable_shared_from_this<MachineBasicBlock> {
  public:
    MachineBasicBlock(BasicBlock *IR_bb, std::weak_ptr<MachineFunction> parent,
                      std::string name = "");
    void insert_instr(std::shared_ptr<MachineInstr> instr,
                      std::vector<std::shared_ptr<MachineInstr>>::iterator it);
    void append_instr(std::shared_ptr<MachineInstr> instr);
    void clear_instrs();
    void add_succ_basic_block(std::weak_ptr<MachineBasicBlock> succ);
    void add_pre_basic_block(std::weak_ptr<MachineBasicBlock> pred);
    std::vector<std::weak_ptr<MachineBasicBlock>> &get_succ_basic_blocks();
    std::vector<std::weak_ptr<MachineBasicBlock>> &get_pre_basic_blocks();
    std::weak_ptr<MachineFunction> get_parent() const;
    std::vector<std::shared_ptr<MachineInstr>> &get_instrs();
    RegisterSet get_def();
    RegisterSet get_use();
    BasicBlock *get_IR_basic_block() const;
    std::string get_name() const;
    std::string print() const;

  private:
    std::string name;
    BasicBlock *IR_bb;
    std::weak_ptr<MachineFunction> parent;
    std::vector<std::shared_ptr<MachineInstr>> instrs;
    std::vector<std::weak_ptr<MachineBasicBlock>> succs;
    std::vector<std::weak_ptr<MachineBasicBlock>> preds;
};