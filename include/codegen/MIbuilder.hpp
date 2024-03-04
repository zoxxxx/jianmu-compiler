#include "MachineBasicBlock.hpp"
#include "MachineInstr.hpp"
#include <cassert>
#include <memory>
class MIBuilder {
  public:
    void setInsertPoint(std::shared_ptr<MachineBasicBlock> block) {
        currentBlock = block;
    }

    void createInstr(/* 参数 */) {
        assert(currentBlock != nullptr && "Insertion block is not set!");
        auto instr = std::make_shared<MachineInstr>(/* 参数 */);
    }

  private:
    std::shared_ptr<MachineBasicBlock> currentBlock;
};
