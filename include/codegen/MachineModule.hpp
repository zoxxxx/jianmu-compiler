#include <memory>
#include <vector>

#include "MachineFunction.hpp"
class MachineModule {
  public:
    MachineModule() = default;
    virtual ~MachineModule() = default;
    virtual void addFunction(std::unique_ptr<MachineFunction> MF) = 0;
    virtual void print() const = 0;
  private:
    std::vector<std::shared_ptr<MachineFunction>> functions;
};