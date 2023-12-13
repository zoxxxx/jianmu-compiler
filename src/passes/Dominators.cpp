#include "Dominators.hpp"
#include "BasicBlock.hpp"

void Dominators::run() {
    for (auto &f1 : m_->get_functions()) {
        auto f = &f1;
        if (f->get_basic_blocks().size() == 0)
            continue;
        for (auto &bb1 : f->get_basic_blocks()) {
            auto bb = &bb1;
            idom_.insert({bb, {}});
            dom_frontier_.insert({bb, {}});
            dom_tree_succ_blocks_.insert({bb, {}});
        }
        
        create_reverse_post_order(f);
        create_idom(f);
        create_dominance_frontier(f);
        create_dom_tree_succ(f);
    }
}

void Dominators::dfs(BasicBlock *bb, std::set<BasicBlock *> &visited) {
    visited.insert(bb);
    for (auto &succ : bb->get_succ_basic_blocks()) {
        if (visited.find(succ) == visited.end()) {
            dfs(succ, visited);
        }
    }
    reverse_post_order_vec_.push_back(bb);
    reverse_post_order_.insert({bb, reverse_post_order_.size()});
}

void Dominators::create_reverse_post_order(Function *f) {
    // 分析得到 f 中各个基本块的逆后序遍历
    std::set<BasicBlock *> visited;
    dfs(f->get_entry_block(), visited);
}

BasicBlock *Dominators::intersect(BasicBlock *bb1, BasicBlock *bb2) {
    while (bb1 != bb2) {
        while (get_reverse_post_order(bb1) < get_reverse_post_order(bb2)) {
            bb1 = get_idom(bb1);
        }
        while (get_reverse_post_order(bb2) < get_reverse_post_order(bb1)) {
            bb2 = get_idom(bb2);
        }
    }
    return bb1;
}

void Dominators::create_idom(Function *f) {
    // 分析得到 f 中各个基本块的 idom
    idom_[f->get_entry_block()] = f->get_entry_block();
    bool changed;
    do {
        changed = false;
        for (auto &bb : reverse_post_order_vec_) {
            if (bb == f->get_entry_block())
                continue;
            BasicBlock *new_idom = nullptr;
            BasicBlock *first_pred = bb->get_pre_basic_blocks().front();
            for (auto &pred : bb->get_pre_basic_blocks()) {
                if(pred == first_pred)
                    continue;
                if(get_idom(pred) != nullptr){
                    new_idom = intersect(pred, new_idom);
                }
            }
            if(new_idom != get_idom(bb)){
                changed = true;
                idom_[bb] = new_idom;
            }
        }
    }
    while(changed);
}

void Dominators::create_dominance_frontier(Function *f) {
    // 分析得到 f 中各个基本块的支配边界集合
    for (auto &bb1 : f->get_basic_blocks()){
        auto bb = &bb1;
        if (bb->get_pre_basic_blocks().size() >= 2){
            for (auto &pred : bb->get_pre_basic_blocks()){
                auto runner = pred;
                while(runner != get_idom(bb)){
                    dom_frontier_[runner].insert(bb);
                    runner = get_idom(runner);
                }
            }
        }
    }
}

void Dominators::create_dom_tree_succ(Function *f) {
    // 分析得到 f 中各个基本块的支配树后继
    for (auto &bb1 : f->get_basic_blocks()){
        auto bb = &bb1;
        if(get_idom(bb) != nullptr){
            dom_tree_succ_blocks_[get_idom(bb)].insert(bb);
        }
    }
}
