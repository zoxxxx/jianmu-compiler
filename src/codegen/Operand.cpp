#include "MachineBasicBlock.hpp"
#include "Operand.hpp"
#include <cassert>
#include <memory>
#include <unordered_map>

std::unordered_map<std::shared_ptr<VirtualRegister>,
                   std::shared_ptr<PhysicalRegister>>
    Register::color = {};

Label::Label(std::weak_ptr<MachineBasicBlock> block) {
    name = block.lock()->get_name();
    this->block = block;
}

Label::Label(std::weak_ptr<MachineFunction> func) {
    name = func.lock()->get_name();
    this->func = func;
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
    regs.push_back(ra());
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
    if(type == RegisterType::General) {
        if(id == 0) 
            return "$zero";
        else if(id == 1)
            return "$ra";
        else if(id == 3)
            return "$sp";
        else if(id >= 4 && id <= 11) 
            return "$a" + std::to_string(id - 4);
        else if(id >= 12 && id <= 20) 
            return "$t" + std::to_string(id - 12);
        else if(id == 22) 
            return "$fp";
        else if(id >= 23 && id <= 31) 
            return "$s" + std::to_string(id - 23);
        else assert(false);
    }
    else if(type == RegisterType::Float) {
        if(id < 8) 
            return "$fa" + std::to_string(id);
        else if(id < 24) 
            return "$ft" + std::to_string(id - 8);
        else if(id < 32)
            return "$fs" + std::to_string(id - 24);
        else assert(false);
    }
    else if(type == RegisterType::FloatCmp) {
        return "$fcc" + std::to_string(id);
    }
    else {
        assert(false);
    }
}