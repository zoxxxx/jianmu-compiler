#pragma once

#include "Module.hpp"

#include <memory>
#include <vector>

class Pass {
  public:
    Pass(Module *m) : m_(m) {}
    virtual ~Pass() = default;
    virtual void run() = 0;
    virtual void print() {}

  protected:
    Module *m_;
};

class PassManager {
  public:
    PassManager(Module *m) : m_(m) {}

    template <typename PassType, typename... Args>
    void add_pass(Args &&...args) {
        passes_.emplace_back(new PassType(m_, std::forward<Args>(args)...));
    }

    void run() {
        for (auto &pass : passes_) {
            pass->run();
        }
    }
    void print() {

        for (auto &pass : passes_) {
            std::cerr << "Pass: ";
            auto &pass_ref = *pass;
            std::cerr << typeid(pass_ref).name() << std::endl;
            pass->print();
        }
    }

  private:
    std::vector<std::unique_ptr<Pass>> passes_;
    Module *m_;
};
