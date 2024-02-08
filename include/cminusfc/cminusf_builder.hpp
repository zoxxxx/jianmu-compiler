#pragma once

#include "BasicBlock.hpp"
#include "Constant.hpp"
#include "Function.hpp"
#include "IRBuilder.hpp"
#include "Module.hpp"
#include "Type.hpp"
#include "ast.hpp"

#include <map>
#include <memory>

struct inherite{
    int DownType;
    int is_empty;
    Value* Data;
    
};

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
    bool push(const std::string& name, Value *val) {
        auto result = inner[inner.size() - 1].insert({name, val});
        return result.second;
    }

    Value *find(const std::string& name) {
        for (auto s = inner.rbegin(); s != inner.rend(); s++) {
            auto iter = s->find(name);
            if (iter != s->end()) {
                return iter->second;
            }
        }

        // Name not found: handled here?
        assert(false && "Name not found in scope");

        return nullptr;
    }

  private:
    std::vector<std::map<std::string, Value *>> inner;
};

class CminusfBuilder : public ASTVisitor {
  public:
    CminusfBuilder() {
        module = std::make_unique<Module>();
        builder = std::make_unique<IRBuilder>(nullptr, module.get());
        auto *TyVoid = module->get_void_type();
        auto *TyInt32 = module->get_int32_type();
        auto *TyFloat = module->get_float_type();

        auto *input_type = FunctionType::get(TyInt32, {});
        auto *input_fun = Function::create(input_type, "input", module.get());

        std::vector<Type *> output_params;
        output_params.push_back(TyInt32);
        auto *output_type = FunctionType::get(TyVoid, output_params);
        auto *output_fun = Function::create(output_type, "output", module.get());

        std::vector<Type *> output_float_params;
        output_float_params.push_back(TyFloat);
        auto *output_float_type = FunctionType::get(TyVoid, output_float_params);
        auto *output_float_fun =
            Function::create(output_float_type, "outputFloat", module.get());

        auto *neg_idx_except_type = FunctionType::get(TyVoid, {});
        auto *neg_idx_except_fun = Function::create(
            neg_idx_except_type, "neg_idx_except", module.get());

        scope.enter();
        scope.push("input", input_fun);
        scope.push("output", output_fun);
        scope.push("outputFloat", output_float_fun);
        scope.push("neg_idx_except", neg_idx_except_fun);
    }

    std::unique_ptr<Module> getModule() { return std::move(module); }

  private:
    virtual inherite visit(ASTProgram &) override final;
    virtual inherite visit(ASTCompUnit &) override final;
    virtual inherite visit(ASTDecl &) override final;
    virtual inherite visit(ASTStmt &) override final;
    virtual inherite visit(ASTFuncDef &) override final;
    virtual inherite visit(ASTConstDecl &) override final;
    virtual inherite visit(ASTVarDecl &) override final;
    virtual inherite visit(ASTBtype &) override final;
    virtual inherite visit(ASTConstDefList &) override final;
    virtual inherite visit(ASTConstDef &) override final;
    virtual inherite visit(ASTIdent &) override final;
    virtual inherite visit(ASTConstExpList &) override final;
    virtual inherite visit(ASTConstInitVal &) override final;
    virtual inherite visit(ASTConstExp &) override final;
    virtual inherite visit(ASTConstInitValList &) override final;

    virtual inherite visit(ASTVarDefList &) override final;
    virtual inherite visit(ASTVarDef &) override final;
    virtual inherite visit(ASTExpList &) override final;
    virtual inherite visit(ASTInitVal &) override final;
    virtual inherite visit(ASTExp &) override final;
    virtual inherite visit(ASTInitValList &) override final;

    virtual inherite visit(ASTFuncType &) override final;
    virtual inherite visit(ASTFuncFParams &) override final;
    virtual inherite visit(ASTFuncRParams &) override final;
    virtual inherite visit(ASTBlock &) override final;
    virtual inherite visit(ASTFuncFParam &) override final;
    virtual inherite visit(ASTBlockItemList &) override final;
    virtual inherite visit(ASTBlockItem &) override final;

    virtual inherite visit(ASTLVal &) override final;
    virtual inherite visit(ASTCond &) override final;
    
    
    virtual inherite visit(ASTAddExp &) override final;
    virtual inherite visit(ASTLOrExp &) override final;

    virtual inherite visit(ASTLOrExp &) override final;
    virtual inherite visit(ASTPrimaryExp &) override final;
    virtual inherite visit(ASTNumber &) override final;
    virtual inherite visit(ASTIntConst &) override final;
    virtual inherite visit(ASTFloatConst &) override final;
    virtual inherite visit(ASTUnaryExp &) override final;
    virtual inherite visit(ASTUnaryOp &) override final;

    virtual inherite visit(ASTMulExp &) override final;
    virtual inherite visit(ASTRelExp &) override final;
    virtual inherite visit(ASTEqExp &) override final;
    virtual inherite visit(ASTLAndExp &) override final;
    
    std::unique_ptr<IRBuilder> builder;
    Scope scope;
    std::unique_ptr<Module> module;

    struct {
        int vartype = 0; //when translating, use vartype to record i or f

        
        // function that is being built
        Function *func = nullptr;
        //  you should add more fields to store state
        bool isLValue = false;
    } context;
};
