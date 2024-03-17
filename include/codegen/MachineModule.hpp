#pragma once

#include <memory>
#include <vector>

#include "MachineFunction.hpp"
#include "Module.hpp"
class MachineFunction;
class MachineModule {
  public:
    MachineModule (Module *IR_module) : IR_module(IR_module) {}
    ~MachineModule() = default;
    void add_function(std::shared_ptr<MachineFunction> MF);
    void print() const;
    std::vector<std::shared_ptr<MachineFunction>> get_functions() const;
  private:
    Module *IR_module;
    std::vector<std::shared_ptr<MachineFunction>> functions;
};