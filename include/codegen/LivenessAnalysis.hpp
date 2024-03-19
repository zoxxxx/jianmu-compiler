#include "BasicBlock.hpp"
#include "MachineModule.hpp"
#include "MachinePass.hpp"
#include "Operand.hpp"
#include <memory>


class LivenessAnalysis : public MachinePass {
  public:
    LivenessAnalysis(std::shared_ptr<MachineModule> MM) : MachinePass(MM) {}
    ~LivenessAnalysis() override = default;
    void run() override;
    std::string print();
    RegisterSet &get_live_in(std::shared_ptr<MachineBasicBlock> block);
    RegisterSet &get_live_out(std::shared_ptr<MachineBasicBlock> block);

  private:
    MBBSetMap live_in;
    MBBSetMap live_out;
};