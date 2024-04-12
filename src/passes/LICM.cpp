#include "BasicBlock.hpp"
#include "Constant.hpp"
#include "GlobalVariable.hpp"
#include "Instruction.hpp"
#include "LICM.hpp"
#include "PassManager.hpp"
#include <memory>

void LoopInvariantCodeMotion::run() {

    loop_detection_ = std::make_unique<LoopDetection>(m_);
    loop_detection_->run();

    for (auto &loop : loop_detection_->get_loops()) {
        is_loop_done_[loop] = false;
    }

    for (auto &loop : loop_detection_->get_loops()) {
        traverse_loop(loop);
    }
}

void LoopInvariantCodeMotion::traverse_loop(std::shared_ptr<Loop> loop) {
    if (is_loop_done_[loop]) {
        return;
    }
    is_loop_done_[loop] = true;
    for (auto &sub_loop : loop->get_sub_loops()) {
        traverse_loop(sub_loop);
    }
    run_on_loop(loop);
}

void LoopInvariantCodeMotion::run_on_loop(std::shared_ptr<Loop> loop) {
    std::set<Value *> loop_exists;
    for (auto &bb : loop->get_blocks()) {
        for (auto &inst : bb->get_instructions()) {
            loop_exists.insert(&inst);
        }
    }

    std::set<Value *> loop_invariant;
    bool changed;
    do {
        changed = false;
        for (auto &bb : loop->get_blocks()) {
            for (auto &inst : bb->get_instructions()) {
                if (loop_invariant.find(&inst) != loop_invariant.end())
                    continue;
                auto inst_type = inst.get_instr_type();
                if (inst_type == Instruction::alloca ||
                    inst_type == Instruction::store ||
                    inst_type == Instruction::load ||
                    inst_type == Instruction::ret ||
                    inst_type == Instruction::br ||
                    inst_type == Instruction::phi)
                    continue;
                // TODO handle call instruction after pure function analysis
                if (inst_type == Instruction::call)
                    continue;

                bool is_invariant = true;
                for (auto &op : inst.get_operands()) {
                    if ((loop_exists.find(op) != loop_exists.end() &&
                         loop_invariant.find(op) == loop_invariant.end()) ||
                        dynamic_cast<GlobalVariable *>(op) != nullptr) {
                        is_invariant = false;
                        break;
                    }
                }
                if (is_invariant) {
                    loop_invariant.insert(&inst);
                    loop_exists.erase(&inst);
                    changed = true;
                }
            }
        }
    } while (changed);

    if (loop->get_preheader() == nullptr) {
        loop->set_preheader(
            BasicBlock::create(m_, "", loop->get_header()->get_parent()));
    }

    // insert preheader
    auto preheader = loop->get_preheader();

    // update phi
    for (auto &phi_inst_ : loop->get_header()->get_instructions()) {
        if (phi_inst_.get_instr_type() != Instruction::phi)
            break;
        auto *phi_inst = dynamic_cast<PhiInst *>(&phi_inst_);

        auto *new_phi_inst = PhiInst::create_phi(phi_inst->get_type(), preheader);
        std::vector<std::pair<Value *, Value *>> to_move; 
        for (unsigned i = 0; i < phi_inst->get_num_operand(); i += 2) {
            auto *val = phi_inst->get_operand(i);
            auto *bb = dynamic_cast<BasicBlock *>(phi_inst->get_operand(i + 1));
            if(loop->get_back_edges_nodes().find(bb) == loop->get_back_edges_nodes().end())
                continue;
            to_move.push_back({val, bb});
        }

        if(to_move.size() == 0)
            continue;

        for (auto &pair : to_move) {
            phi_inst->remove_phi_operand(pair.second);
            new_phi_inst->add_phi_pair_operand(pair.first, pair.second);
        }

        phi_inst->add_phi_pair_operand(new_phi_inst, preheader);
        preheader->add_instruction(new_phi_inst);
    }

    // rebuild cfg and br instruction
    std::vector<BasicBlock *> pred_to_remove;
    for (auto &pred : loop->get_header()->get_pre_basic_blocks()) {
        if (loop->get_back_edges_nodes().find(pred) != loop->get_back_edges_nodes().end())
            continue;
        auto *term = pred->get_terminator();
        for (unsigned i = 0; i < term->get_num_operand(); i++) {
            if (term->get_operand(i) == loop->get_header()) {
                term->set_operand(i, preheader);
            }
        }
        pred->remove_succ_basic_block(loop->get_header());
        pred->add_succ_basic_block(preheader);
        pred_to_remove.push_back(pred);
    }

    for (auto &pred : pred_to_remove) {
        loop->get_header()->remove_pre_basic_block(pred);
    }

    // insert loop invariant
    for (auto &inst : loop_invariant) {
        preheader->add_instruction(dynamic_cast<Instruction *>(inst));
    }

    // insert preheader br to header
    preheader->add_instruction(BranchInst::create_br(loop->get_header(), preheader));
    preheader->add_succ_basic_block(loop->get_header());
    loop->get_header()->add_pre_basic_block(preheader);

    // insert preheader to parent loop
    if(loop->get_parent() != nullptr) {
        loop->get_parent()->add_block(preheader);
    }
}