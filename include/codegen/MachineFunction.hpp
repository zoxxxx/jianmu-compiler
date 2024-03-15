#pragma once

#include "Function.hpp"
#include "MachineBasicBlock.hpp"
#include "MachineModule.hpp"
#include "Module.hpp"
#include "Operand.hpp"
#include "Value.hpp"
#include <memory>
#include <vector>

class FrameScheduler;
class MachineModule;
class MachineBasicBlock;
class MachineFunction : public std::enable_shared_from_this<MachineFunction> {
  public:
    MachineFunction(Function *function, std::weak_ptr<MachineModule> parent)
        : function(function), parent(parent) {
        frame_scheduler = std::make_unique<FrameScheduler>();
    }
    ~MachineFunction() = default;
    void add_basic_block(std::shared_ptr<MachineBasicBlock> bb) {
        basic_blocks.push_back(bb);
    }
    void set_entry_block(std::shared_ptr<MachineBasicBlock> bb) {
        entry_block = bb;
    }
    void set_exit_block(std::shared_ptr<MachineBasicBlock> bb) {
        exit_block = bb;
    }
    std::shared_ptr<MachineBasicBlock> get_entry_block() const {
        return entry_block;
    }
    std::shared_ptr<MachineBasicBlock> get_exit_block() const {
        return exit_block;
    }
    virtual void print() const;
    virtual std::string get_name() const;
    virtual std::vector<std::shared_ptr<MachineBasicBlock>>
    get_basic_blocks() const;
    std::unique_ptr<FrameScheduler> frame_scheduler;

  private:
    Function *function;
    std::weak_ptr<MachineModule> parent;
    std::vector<std::shared_ptr<MachineBasicBlock>> basic_blocks;
    std::shared_ptr<MachineBasicBlock> entry_block;
    std::shared_ptr<MachineBasicBlock> exit_block;
};

class FrameScheduler {
  public:
    void insert_alloca(Value *val) {
        auto alloca_inst = static_cast<AllocaInst *>(val);
        frame_size =
            align(frame_size + alloca_inst->get_alloca_type()->get_size(), 16);
        alloca_map[val] = frame_size;
    }
    void insert_reg(std::shared_ptr<Register> reg, int size) {
        frame_size = align(frame_size + size, size);
        reg_map[reg] = frame_size;
    }
    int get_alloca_offset(Value *val) { return alloca_map[val]; }
    int get_reg_offset(std::shared_ptr<Register> reg) { return reg_map[reg]; }
    int get_frame_size() {
        frame_size = align(frame_size, 16);
        return frame_size;
    }

  private:
    int align(int size, unsigned alignment) {
        return ((size + (alignment - 1)) & ~(alignment - 1));
    }
    int frame_size = 0;
    std::map<Value *, int> alloca_map;
    std::map<std::shared_ptr<Register>, int> reg_map;
};