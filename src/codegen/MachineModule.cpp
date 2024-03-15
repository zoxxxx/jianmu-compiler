#include "MachineModule.hpp"
void MachineModule::add_function(std::shared_ptr<MachineFunction> MF) {
    functions.push_back(MF);
}

std::vector<std::shared_ptr<MachineFunction>> MachineModule::get_functions() const {
    return functions;
}