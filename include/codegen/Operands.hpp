#include <cassert>
#include <functional> // 用于std::function
#include <memory>
#include <utility>

class VirtualRegister;
class PhysicalRegister;

class Operand {
  public:
    Operand() = default;
    virtual ~Operand() = default;
    virtual bool is_reg() const { return false; }
    virtual bool is_imm() const { return false; }
    virtual bool is_mem() const { return false; }
};

class Register : public Operand {
  public:
    enum RegisterType { General, Float, FloatCmp };
    Register() = default;
    virtual ~Register() = default;
    bool is_reg() const override final { return true; }
    virtual bool is_virtual_reg() const { return false; }
    virtual bool is_physical_reg() const { return false; }
    RegisterType get_type() const { return type; }

  protected:
    RegisterType type;
};

class RegisterFactory {
  public:
    static RegisterFactory &get_instance() {
        static RegisterFactory instance;
        return instance;
    }

    std::shared_ptr<VirtualRegister>
    get_new_virtual_reg(Register::RegisterType type) {
        auto reg = std::make_shared<VirtualRegister>(next_virtual_reg_id, type);
        virtual_regs[next_virtual_reg_id] = reg;
        next_virtual_reg_id++;
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

        // 如果寄存器不存在，则创建一个新的寄存器实例
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
    VirtualRegister(unsigned id, RegisterType type) : id(id) {
        this->type = type;
    }
    virtual ~VirtualRegister() = default;
    bool is_virtual_reg() const override final { return true; }
    unsigned get_ID() const { return id; }
    static std::shared_ptr<VirtualRegister> create(RegisterType type) {
        return RegisterFactory::get_instance().get_new_virtual_reg(type);
    }
    static std::shared_ptr<VirtualRegister> get(unsigned id) {
        return RegisterFactory::get_instance().get_virtual_reg_by_id(id);
    }

  private:
    unsigned id;
};

class PhysicalRegister : public Register {
  public:
    PhysicalRegister() = default;
    virtual ~PhysicalRegister() = default;
    bool is_physical_reg() const override final { return true; }

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
        assert(i < 4);
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

  private:
};

class Immediate : public Operand {
  public:
    bool is_imm() const override final{ return true; }
    bool is_imm12() const { return value >= -2048 && value <= 2047; }
  private:
    int value;
};

class Memory : public Operand {
  public:
    bool isMem() const { return true; }
};