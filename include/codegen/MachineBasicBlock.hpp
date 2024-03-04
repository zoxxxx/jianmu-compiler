#pragma once

#include "MachineInstr.hpp"
#include <memory>
#include <vector>

class MachineBasicBlock : public std::enable_shared_from_this<MachineBasicBlock> {
private:
    std::vector<std::unique_ptr<MachineInstr>> instrs;
};