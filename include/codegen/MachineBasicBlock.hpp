#pragma once

#include "MachineInstr.hpp"
#include "Operand.hpp"
#include <memory>
#include <vector>

class Operand;
class MachineInstr;
class MachineBasicBlock : public std::enable_shared_from_this<MachineBasicBlock> {
public:
    void append_instr(std::shared_ptr<MachineInstr> instr) {
        instrs.push_back(instr);
    }
private:
    std::vector<std::shared_ptr<MachineInstr>> instrs;
    // std::vector<std::unique_ptr<MachineInstr>> instrs;
};