#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <unordered_set>

class VirtualRegister;
class PhysicalRegister;
class MachineBasicBlock;
class MachineFunction;
class Immediate;

class Operand : public std::enable_shared_from_this<Operand> {
  public:
    virtual ~Operand() = default;
    virtual bool is_reg() const { return false; }
    virtual bool is_imm() const { return false; }
    virtual bool is_label() const { return false; }
    virtual std::string get_name() const = 0;
};

class Register : public Operand {
  public:
    enum RegisterType { General, Float, FloatCmp };
    enum RegisterFlag { kUSING_SP_AS_FRAME_REG = 1 << 0 };
    Register() = default;
    virtual ~Register() = default;
    bool is_reg() const override final { return true; }
    virtual bool is_virtual_reg() const { return false; }
    virtual bool is_physical_reg() const { return false; }
    virtual std::string get_name() const override = 0;
    RegisterType get_type() const { return type; }
    bool is_using_sp_as_frame_reg() const {
        return flags & kUSING_SP_AS_FRAME_REG;
    }
    static std::unordered_map<std::shared_ptr<VirtualRegister>, std::shared_ptr<PhysicalRegister>> color;
    static std::unordered_set<std::shared_ptr<VirtualRegister>> temp_regs;
  protected:
    RegisterType type;
    unsigned flags;
};

using RegisterSet = std::unordered_set<std::shared_ptr<Register>>;
class RegisterFactory {
  public:
    static RegisterFactory &get_instance() {
        static RegisterFactory instance;
        return instance;
    }

    std::shared_ptr<VirtualRegister>
    get_new_virtual_reg(Register::RegisterType type, unsigned flags) {
        auto reg =
            std::make_shared<VirtualRegister>(next_virtual_reg_id, type, flags);
        virtual_regs[next_virtual_reg_id] = reg;
        next_virtual_reg_id++;
        Register::temp_regs.insert(reg);
        return reg;
    }

    std::shared_ptr<VirtualRegister> get_virtual_reg_by_id(unsigned id) {
        auto it = virtual_regs.find(id);
        if (it != virtual_regs.end()) {
            return it->second;
        }
        return nullptr;
    }

    std::shared_ptr<PhysicalRegister> get_register(Register::RegisterType type,
                                                   unsigned id) {
        auto it = physical_regs.find(std::make_pair(type, id));
        if (it != physical_regs.end()) {
            return it->second;
        }

        auto reg = std::make_shared<PhysicalRegister>(id, type);
        physical_regs[std::make_pair(type, id)] = reg;
        return reg;
    }

  private:
    RegisterFactory() = default;

    unsigned next_virtual_reg_id = 0;
    std::unordered_map<unsigned, std::shared_ptr<VirtualRegister>> virtual_regs;

    struct pair_hash {
        template <class T1, class T2>
        std::size_t operator()(const std::pair<T1, T2> &pair) const {
            auto hash1 = std::hash<T1>{}(pair.first);
            auto hash2 = std::hash<T2>{}(pair.second);
            return hash1 ^ hash2;
        }
    };
    std::unordered_map<std::pair<Register::RegisterType, unsigned>,
                       std::shared_ptr<PhysicalRegister>, pair_hash>
        physical_regs;

    RegisterFactory(const RegisterFactory &) = delete;
    RegisterFactory &operator=(const RegisterFactory &) = delete;
};

class VirtualRegister : public Register {
  public:
    VirtualRegister(unsigned id, RegisterType type, unsigned flags) : id(id) {
        this->type = type;
        this->flags = flags;
    }
    ~VirtualRegister() override = default;
    bool is_virtual_reg() const override final { return true; }
    unsigned get_ID() const { return id; }
    RegisterType get_type() const { return type; }
    static std::shared_ptr<VirtualRegister> create(RegisterType type,
                                                   unsigned flags = 0) {
        return RegisterFactory::get_instance().get_new_virtual_reg(type, flags);
    }
    static std::shared_ptr<VirtualRegister> get(unsigned id) {
        return RegisterFactory::get_instance().get_virtual_reg_by_id(id);
    }
    std::string get_name() const override final ;
  private:
    unsigned id;
};

class PhysicalRegister : public Register {
  public:
    ~PhysicalRegister() override = default;
    PhysicalRegister(unsigned id, RegisterType type) : id(id) {
        this->type = type;
    }
    bool is_physical_reg() const override final { return true; }
    unsigned get_ID() const { return id; }
    RegisterType get_type() const { return type; }
    static std::shared_ptr<PhysicalRegister> zero() {
        return RegisterFactory::get_instance().get_register(
            RegisterType::General, 0);
    }
    static std::shared_ptr<PhysicalRegister> ra() {
        return RegisterFactory::get_instance().get_register(
            RegisterType::General, 1);
    }
    static std::shared_ptr<PhysicalRegister> tp() {
        return RegisterFactory::get_instance().get_register(
            RegisterType::General, 2);
    }
    static std::shared_ptr<PhysicalRegister> sp() {
        return RegisterFactory::get_instance().get_register(
            RegisterType::General, 3);
    }
    static std::shared_ptr<PhysicalRegister> a(unsigned i) {
        assert(i < 8);
        return RegisterFactory::get_instance().get_register(
            RegisterType::General, i + 4);
    }
    static std::shared_ptr<PhysicalRegister> t(unsigned i) {
        assert(i < 9);
        return RegisterFactory::get_instance().get_register(
            RegisterType::General, i + 12);
    }
    static std::shared_ptr<PhysicalRegister> fp() {
        return RegisterFactory::get_instance().get_register(
            RegisterType::General, 22);
    }
    static std::shared_ptr<PhysicalRegister> s(unsigned i) {
        assert(i < 9);
        return RegisterFactory::get_instance().get_register(
            RegisterType::General, i == 9 ? 22 : i + 23);
    }
    static std::shared_ptr<PhysicalRegister> r(unsigned i) {
        assert(i < 32);
        return RegisterFactory::get_instance().get_register(
            RegisterType::General, i);
    }

    // Floating Point Registers
    static std::shared_ptr<PhysicalRegister> f(unsigned i) {
        return RegisterFactory::get_instance().get_register(RegisterType::Float,
                                                            i);
    }
    static std::shared_ptr<PhysicalRegister> fa(unsigned i) {
        assert(i < 8);
        return RegisterFactory::get_instance().get_register(RegisterType::Float,
                                                            i);
    }
    static std::shared_ptr<PhysicalRegister> ft(unsigned i) {
        assert(i < 16);
        return RegisterFactory::get_instance().get_register(RegisterType::Float,
                                                            i + 8);
    }
    static std::shared_ptr<PhysicalRegister> fs(unsigned i) {
        assert(i < 8);
        return RegisterFactory::get_instance().get_register(RegisterType::Float,
                                                            i + 24);
    }

    // Floating Point Compare Registers
    static std::shared_ptr<PhysicalRegister> fcc(unsigned i) {
        assert(i < 8);
        return RegisterFactory::get_instance().get_register(
            Register::RegisterType::FloatCmp, i);
    }

    static RegisterSet allocatable_general() {
        RegisterSet ret;
        ret.insert(ra());
        for(int i = 0; i < 8; i++) {
            ret.insert(a(i));
        }
        for(int i = 0; i < 9; i++) {
            ret.insert(t(i));
        }
        for(int i = 0; i < 9; i++) {
            ret.insert(s(i));
        }
        return ret;
    }

    static RegisterSet allocatable_float() {
        RegisterSet ret;
        for (int i = 0; i < 32; i++) {
            ret.insert(f(i));
        }
        return ret;
    }

    static RegisterSet allocatable_fcc() {
        RegisterSet ret;
        for (int i = 0; i < 8; i++) {
            ret.insert(fcc(i));
        }
        return ret;
    }

    static std::vector<std::shared_ptr<PhysicalRegister>> callee_saved_regs();

    static std::vector<std::shared_ptr<PhysicalRegister>> caller_saved_regs();
    std::string get_name() const override final;

  private:
    unsigned id;
};

class Immediate : public Operand {
  public:
    ~Immediate() override = default;
    static std::shared_ptr<Immediate> create(int value) ;

    bool is_imm_length(int bits) const {
        assert(bits <= 32 && bits > 0);
        return value >= -(1ll << (bits - 1)) &&
               value <= (1ll << (bits - 1)) - 1;
    }

    bool is_uimm_length(int bits) const {
        assert(bits <= 32 && bits > 0);
        return value >= 0 && value <= (1ll << bits) - 1;
    }

    bool is_imm() const override final { return true; }
    std::string get_name() const override final {
        return std::to_string(value);
    }
    Immediate(int value) : value(value) {}
  private:
    int value;
};

class Label : public Operand {
  public:
    Label(std::string name) : name(name) {}
    Label(std::weak_ptr<MachineBasicBlock> block);
    Label(std::weak_ptr<MachineFunction> function);
    ~Label() override = default;
    bool is_label() const override final { return true; }
    std::string get_name() const override final { return name; }
    std::weak_ptr<MachineBasicBlock> get_block() const {
        assert(!block.expired());
        return block;
    }
    std::weak_ptr<MachineFunction> get_function() const {
        assert(!func.expired());
        return func;
    }

  private:
    std::weak_ptr<MachineBasicBlock> block;
    std::weak_ptr<MachineFunction> func;
    std::string name;
};