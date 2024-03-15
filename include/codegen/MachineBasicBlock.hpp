#pragma once

#include "BasicBlock.hpp"
#include "MachineInstr.hpp"
#include "MachineModule.hpp"
#include "Operand.hpp"
#include <memory>
#include <vector>

class Operand;
class MachineInstr;
class MachineFunction;
class MachineBasicBlock : public std::enable_shared_from_this<MachineBasicBlock> {
public:
    MachineBasicBlock(BasicBlock *IR_bb, std::weak_ptr<MachineFunction> parent, std::string name = "") : IR_bb(IR_bb), parent(parent) {
        if(IR_bb != nullptr) {
            name = parent.lock()->get_name() + "_label_" + IR_bb->get_name();
        }
        else { 
            name = parent.lock()->get_name() + name;
        }
    } 
    void insert_instr(std::shared_ptr<MachineInstr> instr, std::vector<std::shared_ptr<MachineInstr>>::iterator it) {
        instrs.insert(it, instr);
    }
    void append_instr(std::shared_ptr<MachineInstr> instr) {
        instrs.push_back(instr);
    }
    void add_succ_basic_block(std::shared_ptr<MachineBasicBlock> succ) {
        succs.push_back(succ);
    }
    void add_pre_basic_block(std::shared_ptr<MachineBasicBlock> pred) {
        preds.push_back(pred);
    }
    std::vector<std::shared_ptr<MachineInstr>> get_instrs() const {
        return instrs;
    }
    BasicBlock *get_IR_basic_block() const {
        assert(IR_bb != nullptr && "IR_basic_block is nullptr");
        return IR_bb;
    }
    std::string get_name() const {
        return name;
    }
private:
    std::string name;
    BasicBlock *IR_bb;
    std::weak_ptr<MachineFunction> parent;
    std::vector<std::shared_ptr<MachineInstr>> instrs;
    std::vector<std::weak_ptr<MachineBasicBlock>> succs;
    std::vector<std::weak_ptr<MachineBasicBlock>> preds;
};