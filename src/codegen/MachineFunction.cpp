#include "MachineFunction.hpp"
#include "Operand.hpp"

std::string MachineFunction::get_name() const { return function->get_name(); }

std::vector<std::shared_ptr<MachineBasicBlock>>
MachineFunction::get_basic_blocks() const {
    return basic_blocks;
}