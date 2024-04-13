#include "Dominators.hpp"
#include "LoopDetection.hpp"
#include <memory>

void LoopDetection::run() {
    dominators_ = std::make_unique<Dominators>(m_);
    for (auto &f1 : m_->get_functions()) {
        auto f = &f1;
        func_ = f;
        run_on_func(f);
    }
}

void LoopDetection::discover_loop_and_sub_loops(BasicBlock *bb,
                                                BBset &back_edges_nodes,
                                                std::shared_ptr<Loop> loop) {
    BBvec work_list = {back_edges_nodes.begin(), back_edges_nodes.end()};
    while (!work_list.empty()) {
        auto bb = work_list.back();
        work_list.pop_back();
        if (bb_to_loop_.find(bb) == bb_to_loop_.end()) {
            loop->add_block(bb);
            bb_to_loop_[bb] = loop;
            for (auto &pred : bb->get_pre_basic_blocks()) {
                work_list.push_back(pred);
            }
        } else if(bb_to_loop_[bb] != loop) {
            auto sub_loop = bb_to_loop_[bb];
            while(sub_loop->get_parent() != nullptr) {
                sub_loop = sub_loop->get_parent();
            }
            if(sub_loop == loop) {
                continue;
            }
            sub_loop->set_parent(loop);
            loop->add_sub_loop(sub_loop);
            for (auto &bb1 : sub_loop->get_header()->get_pre_basic_blocks()) {
                work_list.push_back(bb1);
            }
        }
    }
}

void LoopDetection::run_on_func(Function *f) {
    dominators_->run_on_func(f);
    for (auto &bb1 : dominators_->get_dom_post_order()) {
        auto bb = bb1;
        BBset back_edges_nodes;
        for (auto &pred : bb->get_pre_basic_blocks()) {
            if (dominators_->is_dominate(bb, pred)) {
                // pred is a back edge
                back_edges_nodes.insert(pred);
            }
        }
        if (back_edges_nodes.empty()) {
            continue;
        }
        // create loop
        auto loop = std::make_shared<Loop>(bb);
        bb_to_loop_[bb] = loop;
        for (auto &back_edge_node : back_edges_nodes) {
            loop->add_back_edge_node(back_edge_node);
        }
        loops_.push_back(loop);
        discover_loop_and_sub_loops(bb, back_edges_nodes, loop);
    }
}

void LoopDetection::print() {
    std::cerr<<"Loop Detection Result:"<<std::endl;
    for (auto &loop : loops_) {
        std::cerr << "Loop header: " << loop->get_header()->get_name() << std::endl;
        std::cerr << "Loop blocks: ";
        for (auto &bb : loop->get_blocks()) {
            std::cerr << bb->get_name() << " ";
        }
        std::cerr << std::endl;
        std::cerr << "Sub loops: ";
        for (auto &sub_loop : loop->get_sub_loops()) {
            std::cerr << sub_loop->get_header()->get_name() << " ";
        }
        std::cerr << std::endl;
    }
}