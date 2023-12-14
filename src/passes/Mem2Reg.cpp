#include "Mem2Reg.hpp"
#include "IRBuilder.hpp"
#include "Instruction.hpp"
#include "Value.hpp"

#include <memory>
#include <vector>
#include <queue>

void Mem2Reg::run() {
    // 创建支配树分析 Pass 的实例
    dominators_ = std::make_unique<Dominators>(m_);
    // 建立支配树
    dominators_->run();
    // 以函数为单元遍历实现 Mem2Reg 算法
    for (auto &f : m_->get_functions()) {
        if (f.is_declaration())
            continue;
        func_ = &f;
        if (func_->get_basic_blocks().size() >= 1) {
            // 对应伪代码中 phi 指令插入的阶段
            generate_phi();
            // 对应伪代码中重命名阶段
            rename(func_->get_entry_block());
        }
        // 后续 DeadCode 将移除冗余的局部变量的分配空间
    }
}

void Mem2Reg::generate_phi() {
    // 步骤一：找到活跃在多个 block 的全局名字集合，以及它们所属的 bb 块
    std::map<Value *, unsigned int> alloca_names;
    for (auto & bb1:func_->get_basic_blocks()) {
        auto bb = &bb1;
        for (auto & inst1:bb->get_instructions()) {
            auto inst = &inst1;
            if(inst->is_alloca() && is_valid_ptr(inst)) {
                alloca_names[inst] = 0;
                assert(inst->get_parent() == bb);
            }
            if(inst->is_store()) {
                auto store = dynamic_cast<StoreInst *>(inst);
                auto lval = store->get_lval();
                if (alloca_names.find(lval) != alloca_names.end() && ++ alloca_names[lval] == 2){
                    globals_names_.insert(inst);
                }
            }
        }
    }
    // 步骤二：从支配树获取支配边界信息，并在对应位置插入 phi 指令
    for (auto &inst : globals_names_){
        std::set<BasicBlock *> insert_pos;
        std::queue<BasicBlock *> define_pos;
        insert_pos.insert(inst->get_parent());
        while (!define_pos.empty()){
            auto bb = define_pos.front();
            define_pos.pop();
            for (auto & frontier : dominators_->get_dominance_frontier(bb)){
                if (insert_pos.find(frontier) == insert_pos.end()){
                    auto phi = PhiInst::create_phi(inst->get_type(), frontier);
                    frontier->add_instr_begin(phi);
                    phi_map_[phi] = inst;
                    insert_pos.insert(frontier);
                    if(frontier != inst->get_parent())
                        define_pos.push(frontier);
                }
            }
        }
    }
}

void Mem2Reg::rename(BasicBlock *bb) {
    for (auto &inst1 : bb->get_instructions()){
        auto inst = &inst1;

        // 步骤三：将 phi 指令作为 lval 的最新定值，lval 即是为局部变量 alloca 出的地址空间
        if(inst->is_phi()){
            auto phi = dynamic_cast<PhiInst *>(inst);
            auto lval = phi_map_[phi];
            if(var_stack_.find(lval) == var_stack_.end()){
                var_stack_[lval] = std::stack<Value *>();
            }
            var_stack_[lval].push(phi);
        }

        // 步骤四：用 lval 最新的定值替代对应的load指令
        if(inst->is_load() && globals_names_.find(inst) != globals_names_.end()){
            auto load = dynamic_cast<LoadInst *>(inst);
            auto lval = var_stack_[load->get_lval()].top();
            load->replace_all_use_with(lval);
        }

        // 步骤五：将 store 指令的 rval，也即被存入内存的值，作为 lval 的最新定值
        if(inst->is_store() && globals_names_.find(inst) != globals_names_.end()){
            auto store = dynamic_cast<StoreInst *>(inst);
            auto lval = store->get_lval();
            auto rval = store->get_rval();
            if(var_stack_.find(lval) == var_stack_.end()){
                var_stack_[lval] = std::stack<Value *>();
            }
            var_stack_[lval].push(rval);
        }
    }
    // 步骤六：为 lval 对应的 phi 指令参数补充完整
    for (auto &bb_succ : bb->get_succ_basic_blocks()){
        for (auto &inst1 : bb_succ->get_instructions()){
            auto inst = &inst1;
            if(inst->is_phi()){
                auto phi = dynamic_cast<PhiInst *>(inst);
                auto lval = phi_map_[phi];
                auto rval = var_stack_[lval].top();
                phi->add_phi_pair_operand(rval, bb);
            }
        }
    }

    // 步骤七：对 bb 在支配树上的所有后继节点，递归执行 re_name 操作
    for (auto &bb_succ : dominators_->get_dom_tree_succ_blocks(bb)){
        rename(bb_succ);
    }

    // 步骤八：pop出 lval 的最新定值
    for (auto &inst1 : bb->get_instructions()){
        auto inst = &inst1;
        if(inst->is_phi()){
            auto phi = dynamic_cast<PhiInst *>(inst);
            auto lval = phi_map_[phi];
            var_stack_[lval].pop();
        }
        if(inst->is_store() && globals_names_.find(inst) != globals_names_.end()){
            auto store = dynamic_cast<StoreInst *>(inst);
            auto lval = store->get_lval();
            var_stack_[lval].pop();
        }
    }
    // 步骤九：清除冗余的指令
    for (auto &inst1 : bb->get_instructions()){
        auto inst = &inst1;
        if((inst->is_store() || inst->is_load()) and globals_names_.find(inst) != globals_names_.end()){
            bb->erase_instr(inst);
        }
    }
}
