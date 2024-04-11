#include "MachinePass.hpp"
#include <list>
#include <memory>
class MachineInstr;
class MachineBasicBlock;
using inst_it = std::list<std::shared_ptr<MachineInstr>>::iterator;
class PeepholeOptimization : public MachinePass {
  public:
    PeepholeOptimization(std::shared_ptr<MachineModule> MM) : MachinePass(MM) {}
    ~PeepholeOptimization() override = default;
    void run_on_func(std::shared_ptr<MachineFunction> func);
    void run() override;

  private:
    std::shared_ptr<MachineBasicBlock> mbb;
    template <typename T> inst_it find_pre_inst(inst_it inst, T pred);
    void opt_mul(inst_it &inst_);
    void opt_div(inst_it &inst_);
    void opt_add(inst_it &inst_);
    bool check_const(std::shared_ptr<MachineInstr> inst, int &imm);
};