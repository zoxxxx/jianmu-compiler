#pragma once

#include <memory>
#include <vector>
#include "Function.hpp"
#include "MachineBasicBlock.hpp"

class FrameScheduler;
class FrameObject;

class MachineFunction : public std::enable_shared_from_this<MachineFunction> {
  public:
    MachineFunction() = default;
    virtual ~MachineFunction() = default;
    virtual void print() const = 0;

  private:
    Function *function;

    std::unique_ptr<FrameScheduler> frame_scheduler;
    std::vector<std::shared_ptr<MachineBasicBlock>> basic_blocks;
};

class FrameScheduler {
  private:
    std::vector<std::unique_ptr<FrameObject>> frame_objects;
};

class FrameObject {
  public:
    FrameObject() = default;
    virtual ~FrameObject() = default;
};