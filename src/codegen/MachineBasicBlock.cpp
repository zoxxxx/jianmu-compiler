
#include "MachineBasicBlock.hpp"
#include "MachineFunction.hpp"
MachineBasicBlock::MachineBasicBlock(BasicBlock *IR_bb,
                                     std::weak_ptr<MachineFunction> parent,
                                     std::string name)
    : IR_bb(IR_bb), parent(parent) {
    if (IR_bb != nullptr) {
        name = parent.lock()->get_name() + "_label_" + IR_bb->get_name();
    } else {
        name = parent.lock()->get_name() + name;
    }
}

void MachineBasicBlock::insert_instr(
    std::shared_ptr<MachineInstr> instr,
    std::vector<std::shared_ptr<MachineInstr>>::iterator it) {
    instrs.insert(it, instr);
}
void MachineBasicBlock::append_instr(std::shared_ptr<MachineInstr> instr) {
    instrs.push_back(instr);
}
void MachineBasicBlock::clear_instrs() { instrs.clear(); }
void MachineBasicBlock::add_succ_basic_block(std::shared_ptr<MachineBasicBlock> succ) {
    succs.push_back(succ);
}
void MachineBasicBlock::add_pre_basic_block(std::shared_ptr<MachineBasicBlock> pred) {
    preds.push_back(pred);
}
std::vector<std::shared_ptr<MachineInstr>>& MachineBasicBlock::get_instrs() { return instrs; }
BasicBlock* MachineBasicBlock::get_IR_basic_block() const {
    assert(IR_bb != nullptr && "IR_basic_block is nullptr");
    return IR_bb;
}
std::string MachineBasicBlock::get_name() const { return name; }
std::string MachineBasicBlock::print() const {
    std::string ret;
    ret += get_name() + ":\n";
    for (auto &instr : instrs) {
        ret += instr->print();
    }
    return ret;
}