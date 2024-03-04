#pragma once

#include <memory>
class MachineInstr : public std::enable_shared_from_this<MachineInstr>{
public:
    enum Opcode {
        ADD,
        ADDI,
        SUB,
        MUL,
        DIV,
        MOD,
        LOAD,
        STORE,
        RET,
        CALL,
        PHI,
        BR,

    };
};