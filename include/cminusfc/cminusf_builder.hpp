#pragma once

#include <map>
#include <memory>
#include <utility>

#include "BasicBlock.hpp"
#include "Constant.hpp"
#include "Function.hpp"
#include "IRBuilder.hpp"
#include "Module.hpp"
#include "Type.hpp"
#include "Value.hpp"
#include "ast.hpp"

class Scope {
  public:
    // enter a new scope
    void enter() { inner.emplace_back(); }

    // exit a scope
    void exit() { inner.pop_back(); }

    bool in_global() { return inner.size() == 1; }

    // push a name to scope
    // return true if successful
    // return false if this name already exits
    bool push(const std::string &name, Value *val, bool is_const_decl = false) {
        auto result = inner[inner.size() - 1].insert(
            {name, std::make_pair(val, is_const_decl)});
        return result.second;
    }

    std::pair<Value *, bool> find(const std::string &name) {
        for (auto s = inner.rbegin(); s != inner.rend(); s++) {
            auto iter = s->find(name);
            if (iter != s->end()) {
                return iter->second;
            }
        }

        // Name not found: handled here?
        assert(false && "Name not found in scope");
        return std::make_pair(nullptr, false);
    }

  private:
    std::vector<std::map<std::string, std::pair<Value *, bool>>> inner;
};

struct InitValCalc {
    Type *type;
    std::vector<int> array_size;
    std::vector<int> suffix_product;
    std::vector<Value *> vals;
    Value *single_val;
    int cur_idx = 0;
    bool is_const;
    bool is_single_exp;
    InitValCalc(Module *module, IRBuilder *builder, std::vector<int> array_size,
                Type *type, bool is_const, bool is_single_exp) {
        suffix_product = array_size;
        for (int i = (int)array_size.size() - 2; i >= 0; i--) {
            suffix_product[i] = suffix_product[i] * suffix_product[i + 1];
        }
        vals = std::vector<Value *>(suffix_product[0],
                                    ConstantInt::get(0, module));
        this->type = type;
        this->is_const = is_const;
        this->is_single_exp = is_single_exp;
        cur_idx = 0;
        single_val = nullptr;
    }
    bool is_single_val() { return is_single_exp; }
    virtual Constant *get_global_value(Module *module) = 0;
    virtual void store_value(Module *module, IRBuilder *builder,
                             Value *alloca_inst) = 0;
};

class CminusfBuilder : public ASTVisitor {
  public:
    CminusfBuilder() {
        module = std::make_unique<Module>();
        builder = std::make_unique<IRBuilder>(nullptr, module.get());

        auto *TyVoid = module->get_void_type();
        auto *TyInt32 = module->get_int32_type();
        auto *TyFloat = module->get_float_type();
        auto *TyInt32Ptr = module->get_int32_ptr_type();
        auto *TyFloatPtr = module->get_float_ptr_type();

        auto *getint_type = FunctionType::get(TyInt32, {});
        auto *getint_fun =
            Function::create(getint_type, "getint", module.get());

        auto *getch_type = FunctionType::get(TyInt32, {});
        auto *getch_fun = Function::create(getch_type, "getch", module.get());

        auto *getfloat_type = FunctionType::get(TyFloat, {});
        auto *getfloat_fun =
            Function::create(getfloat_type, "getfloat", module.get());

        auto *getarray_type = FunctionType::get(TyInt32, {TyInt32Ptr});
        auto *getarray_fun =
            Function::create(getarray_type, "getarray", module.get());

        auto *getfarray_type = FunctionType::get(TyInt32, {TyFloatPtr});
        auto *getfarray_fun =
            Function::create(getfarray_type, "getfarray", module.get());

        auto *putint_type = FunctionType::get(TyVoid, {TyInt32});
        auto *putint_fun =
            Function::create(putint_type, "putint", module.get());

        auto *putch_type = FunctionType::get(TyVoid, {TyInt32});
        auto *putch_fun = Function::create(putch_type, "putch", module.get());

        auto *putfloat_type = FunctionType::get(TyVoid, {TyFloat});
        auto *putfloat_fun =
            Function::create(putfloat_type, "putfloat", module.get());

        auto *putarray_type = FunctionType::get(TyVoid, {TyInt32, TyInt32Ptr});
        auto *putarray_fun =
            Function::create(putarray_type, "putarray", module.get());

        auto *putfarray_type = FunctionType::get(TyVoid, {TyInt32, TyFloatPtr});
        auto *putfarray_fun =
            Function::create(putfarray_type, "putfarray", module.get());

        auto *starttime_type = FunctionType::get(TyVoid, {});
        auto *starttime_fun =
            Function::create(starttime_type, "starttime", module.get());

        auto *stoptime_type = FunctionType::get(TyVoid, {});
        auto *stoptime_fun =
            Function::create(stoptime_type, "stoptime", module.get());

        scope.push("getint", getint_fun);
        scope.push("getch", getch_fun);
        scope.push("getfloat", getfloat_fun);
        scope.push("getarray", getarray_fun);
        scope.push("getfarray", getfarray_fun);
        scope.push("putint", putint_fun);
        scope.push("putch", putch_fun);
        scope.push("putfloat", putfloat_fun);
        scope.push("putarray", putarray_fun);
        scope.push("putfarray", putfarray_fun);
        scope.push("starttime", starttime_fun);
        scope.push("stoptime", stoptime_fun);
    }

    std::unique_ptr<Module> getModule() { return std::move(module); }
    virtual void store_const_array(Value *alloca_inst,
                                   std::vector<Value *> idxs,
                                   std::vector<ConstantInt *> array_size,
                                   ConstantArray *vals) = 0;

  private:
    virtual Value *visit(ASTProgram &) override final;
    virtual Value *visit(ASTConstDecl &) override final;
    virtual Value *visit(ASTConstDef &) override final;
    virtual Value *visit(ASTConstInitVal &) override final;
    virtual Value *visit(ASTVarDecl &) override final;
    virtual Value *visit(ASTVarDef &) override final;
    virtual Value *visit(ASTInitVal &) override final;
    virtual Value *visit(ASTFuncDef &) override final;
    virtual Value *visit(ASTFParam &) override final;
    virtual Value *visit(ASTBlock &) override final;
    virtual Value *visit(ASTExpStmt &) override final;
    virtual Value *visit(ASTAssignStmt &) override final;
    virtual Value *visit(ASTBlockStmt &) override final;
    virtual Value *visit(ASTSelectionStmt &) override final;
    virtual Value *visit(ASTIterationStmt &) override final;
    virtual Value *visit(ASTReturnStmt &) override final;
    virtual Value *visit(ASTBreakStmt &) override final;
    virtual Value *visit(ASTContinueStmt &) override final;
    virtual Value *visit(ASTLVal &) override final;
    virtual Value *visit(ASTNumber &) override final;
    virtual Value *visit(ASTUnaryExp &) override final;
    virtual Value *visit(ASTBinaryExp &) override final;
    virtual Value *visit(ASTConstExp &) override final;
    virtual Value *visit(ASTCond &) override final;

    std::unique_ptr<IRBuilder> builder;
    Scope scope;
    std::unique_ptr<Module> module;

    struct {
        // function that is being built
        Function *func = nullptr;
        //  you should add more fields to store state
        bool is_l_value = false;
        bool is_func_block = false;
        Type *type_return = nullptr;
        std::shared_ptr<InitValCalc> init_val_calc;
        BasicBlock *trueBB = nullptr;
        BasicBlock *falseBB = nullptr;
        BasicBlock *condBB = nullptr;
        BasicBlock *iteration_endBB = nullptr;
    } context;
};
