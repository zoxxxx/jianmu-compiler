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
    virtual void add_function(std::shared_ptr<MachineFunction> MF) = 0;
    virtual void print() const = 0;
    virtual std::vector<std::shared_ptr<MachineFunction>> get_functions() const = 0;
  private:
    Module *IR_module;
    std::vector<std::shared_ptr<MachineFunction>> functions;
};