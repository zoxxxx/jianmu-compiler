#pragma once
#include "BasicBlock.hpp"
#include "Function.hpp"
#include "MIBuilder.hpp"
#include "MachineFunction.hpp"
#include "MachineInstr.hpp"
#include "MachinePass.hpp"
#include "Module.hpp"
#include "Operand.hpp"
#include <memory>
class InstructionSelector : public MachinePass {
  public:
    InstructionSelector(std::shared_ptr<MachineModule> machine_module)
        : MachinePass(machine_module) {}
    void run() override;

  private:
    void gen_inst();

    void gen_store_params();

    void gen_ret();
    void gen_br();
    void gen_binary();
    void gen_float_binary();
    void gen_alloca();
    void gen_load();
    void gen_store();
    void gen_icmp();
    void gen_fcmp();
    void gen_zext();
    void gen_call();
    void gen_gep();
    void gen_sitofp();
    void gen_fptosi();
    void gen_bitcast();
    void gen_phi();

    struct {
        std::shared_ptr<MachineFunction> machine_func;           // 当前函数
        std::shared_ptr<MachineBasicBlock> machine_bb = nullptr; // 当前指令
        Instruction *IR_inst = nullptr;                          // 当前指令
        std::unordered_map<Value *, std::shared_ptr<Register>> val_map{}; // 值到寄存器的映射

        void clear() {
            machine_func = nullptr;
            machine_bb = nullptr;
            IR_inst = nullptr;
            val_map.clear();
        }

    } context;
    Module *IR_module;
    MIBuilder builder;
    std::map<Function *, std::shared_ptr<MachineFunction>> func_map;
    std::map<BasicBlock *, std::shared_ptr<MachineBasicBlock>> bb_map;
};