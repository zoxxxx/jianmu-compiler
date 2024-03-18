#pragma once

#include <memory>
#include <vector>

#include "MachineFunction.hpp"
#include "Module.hpp"

class MachineFunction;
class MachineModule {
  public:
    MachineModule(Module *IR_module) : IR_module(IR_module) {}
    ~MachineModule() = default;
    void add_function(std::shared_ptr<MachineFunction> MF);

    void create_init_val(Constant *init, std::string &s, bool &is_first) const;
    std::string global_var_print() const;
    std::string print() const;
    std::vector<std::shared_ptr<MachineFunction>> get_functions() const;
    Module *get_IR_module() const { return IR_module; }

  private:
    Module *IR_module;
    std::vector<std::shared_ptr<MachineFunction>> functions;
};