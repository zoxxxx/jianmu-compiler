#include "MachineBasicBlock.hpp"
#include "Operand.hpp"

Label::Label(std::weak_ptr<MachineBasicBlock> block) {
    name = block.lock()->get_name();
}

std::shared_ptr<Immediate> Immediate::create(int value) {
    static std::map<int, std::shared_ptr<Immediate>> pool;
    auto it = pool.find(value);
    if (it != pool.end()) {
        return it->second;
    } else {
        auto obj = std::make_shared<Immediate>(value);
        pool[value] = obj;
        return obj;
    }
}

std::string VirtualRegister::get_name() const {
    std::string ret = "$";
    switch (type) {
    case RegisterType::General:
        ret += "v_r";
        break;
    case RegisterType::Float:
        ret += "v_f";
        break;
    case RegisterType::FloatCmp:
        ret += "v_fcc";
        break;
    }
    return ret + std::to_string(id);
}

std::vector<std::shared_ptr<PhysicalRegister>>
PhysicalRegister::callee_saved_regs() {
    std::vector<std::shared_ptr<PhysicalRegister>> regs;
    for (int i = 0; i <= 7; i++) {
        regs.push_back(s(i));
    }

    for (int i = 0; i <= 7; i++) {
        regs.push_back(fs(i));
    }
    return regs;
}

std::vector<std::shared_ptr<PhysicalRegister>>
PhysicalRegister::caller_saved_regs() {
    std::vector<std::shared_ptr<PhysicalRegister>> regs;
    for (int i = 0; i <= 7; i++) {
        regs.push_back(a(i));
    }
    for (int i = 0; i <= 8; i++) {
        regs.push_back(t(i));
    }

    for (int i = 0; i <= 7; i++) {
        regs.push_back(fa(i));
    }
    for (int i = 0; i <= 15; i++) {
        regs.push_back(ft(i));
    }

    for (int i = 0; i <= 7; i++) {
        regs.push_back(fcc(i));
    }
    return regs;
}

std::string PhysicalRegister::get_name() const {
    std::string ret = "$";
    switch (type) {
    case RegisterType::General:
        ret += "r";
        break;
    case RegisterType::Float:
        ret += "f";
        break;
    case RegisterType::FloatCmp:
        ret += "fcc";
        break;
    }
    return ret + std::to_string(id);
}