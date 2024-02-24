#include "Constant.hpp"
#include "Function.hpp"
#include "GlobalVariable.hpp"
#include "Instruction.hpp"
#include "Module.hpp"
#include "Type.hpp"
#include "Value.hpp"
#include "ast.hpp"
#include "cminusf_builder.hpp"
#include "logging.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>
#include <vector>

#define CONST_FP(num) ConstantFP::get((float)num, module.get())
#define CONST_INT(num) ConstantInt::get(num, module.get())

// types
Type *VOID_T;
Type *INT_T;
Type *INTPTR_T;
Type *FLOAT_T;
Type *FLOATPTR_T;
Type *BOOL_T;

/*
 * use CMinusfBuilder::Scope to construct scopes
 * scope.enter: enter a new scope
 * scope.exit: exit current scope
 * scope.push: add a new binding to current scope
 * scope.find: find and return the value bound to the name
 */

void InitValCalc::store_value(Module *module, IRBuilder *builder,
                              Value *alloca_inst) {
    if (single_val != nullptr) {
        if (single_val->get_type()->is_float_type() && type->is_integer_type())
            assert(false && "trying to initialize integer with float value");
        else if (single_val->get_type()->is_integer_type() &&
                 type->is_float_type())
            single_val = builder->create_sitofp(single_val, type);
        builder->create_store(single_val, alloca_inst);
        return;
    }
    for (int i = 0; i < (int)suffix_product[0]; i++) {
        if (vals[i]->get_type()->is_float_type() && type->is_integer_type())
            assert(false &&
                   "trying to initialize integer array with float value");
        else if (vals[i]->get_type()->is_integer_type() &&
                 type->is_float_type())
            vals[i] = builder->create_sitofp(vals[i], type);

        if (is_const && dynamic_cast<Constant *>(vals[i]) == nullptr)
            assert(false && "initializer is not a constant");
        std::vector<Value *> idxs;
        int idx = i;
        for (int j = 0; j < (int)array_size.size(); j++) {
            idxs.push_back(ConstantInt::get(idx / suffix_product[j], module));
            idx = idx % suffix_product[j];
        }
        idxs.push_back(ConstantInt::get(idx, module));
        auto ptr = builder->create_gep(alloca_inst, idxs);
        builder->create_store(vals[i], ptr);
    }
}

Constant *InitValCalc::get_const_value(Module *module) {
    if (single_val != nullptr) {
        auto single_val = dynamic_cast<Constant *>(this->single_val);
        if (single_val == nullptr)
            assert(false && "initializer is not a constant");

        if (dynamic_cast<ConstantInt *>(single_val) == nullptr &&
            type->is_integer_type())
            assert(false && "trying to initialize integer with float value");
        else if (dynamic_cast<ConstantFP *>(single_val) == nullptr &&
                 type->is_float_type())
            return ConstantFP::get(
                (float)(dynamic_cast<ConstantInt *>(single_val)->get_value()),
                module);
        else
            return dynamic_cast<Constant *>(single_val);
    }

    std::vector<Constant *> constant_array;
    for (int i = 0; i < (int)suffix_product[0]; i++) {
        auto val = dynamic_cast<Constant *>(vals[i]);
        if (val == nullptr)
            assert(false && "initializer is not a constant");
        if (dynamic_cast<ConstantInt *>(val) == nullptr &&
            type->is_integer_type())
            assert(false && "trying to initialize integer with float value");
        else if (dynamic_cast<ConstantFP *>(val) == nullptr &&
                 type->is_float_type())
            return ConstantFP::get(
                (float)(dynamic_cast<ConstantInt *>(val)->get_value()), module);
        else
            constant_array.push_back(val);
    }

    std::vector<Constant *> new_constant_array;
    for (int i = (int)array_size.size() - 1; i >= 0; i--) {
        std::vector<Constant *> sub_constant_array;
        for (int j = 0; j < (int)constant_array.size(); j++) {
            sub_constant_array.push_back(constant_array[j]);
            if ((j + 1) % array_size[i] == 0) {
                // std::cerr << "j=" << j << std::endl;
                // std::cerr << "array_size[i]=" << array_size[i] << std::endl;
                new_constant_array.push_back(ConstantArray::get(
                    ArrayType::get(sub_constant_array[0]->get_type(),
                                   array_size[i]),
                    sub_constant_array));
                sub_constant_array.clear();
            }
        }
        constant_array = new_constant_array;
        new_constant_array.clear();
    }
    assert(constant_array.size() == 1 && "constant array size error");
    return constant_array[0];
}

Value *CminusfBuilder::visit(ASTProgram &node) {
    VOID_T = module->get_void_type();
    INT_T = module->get_int32_type();
    INTPTR_T = module->get_int32_ptr_type();
    FLOAT_T = module->get_float_type();
    FLOATPTR_T = module->get_float_ptr_type();
    BOOL_T = module->get_int1_type();
    for (auto &def_or_decl : node.defs_and_decls) {
        std::visit([this](auto &&arg) { arg->accept(*this); }, def_or_decl);
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTConstDecl &node) {
    for (auto &def : node.const_defs) {
        def->accept(*this);
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTVarDecl &node) {
    for (auto &def : node.var_defs) {
        def->accept(*this);
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTConstDef &node) {
    Type *var_type;

    if (node.type == TYPE_INT) {
        var_type = INT_T;
    } else if (node.type == TYPE_FLOAT) {
        var_type = FLOAT_T;
    } else {
        assert(false && "Unknown type");
    }

    std::vector<int> array_size;
    for (auto exp = node.array_size.rbegin(); exp != node.array_size.rend();
         exp++) {
        auto const_ptr = dynamic_cast<ConstantInt *>((*exp)->accept(*this));
        assert(const_ptr != nullptr && "Array size must be a constant integer");
        array_size.push_back(const_ptr->get_value());
        assert(const_ptr->get_value() > 0 && "Array size must be positive");
        var_type = ArrayType::get(var_type, const_ptr->get_value());
    }

    array_size = std::vector<int>(array_size.rbegin(), array_size.rend());
    if (node.init_val != nullptr) {

        context.init_val_calc = std::make_shared<InitValCalc>(
            module.get(), builder.get(), array_size,
            node.type == TYPE_INT ? INT_T : FLOAT_T, true,
            node.init_val->is_single_exp);
        node.init_val->accept(*this);

        if (scope.in_global()) {
            scope.push(
                node.id,
                GlobalVariable::create(
                    node.id, module.get(), var_type, true,
                    context.init_val_calc->get_const_value(module.get())),
                true);
            const_scope.push(
                node.id, context.init_val_calc->get_const_value(module.get()));
        } else {
            auto alloca_inst = builder->create_alloca(var_type);
            scope.push(node.id, alloca_inst, true);
            context.init_val_calc->store_value(module.get(), builder.get(),
                                               alloca_inst);
            const_scope.push(
                node.id, context.init_val_calc->get_const_value(module.get()));
        }
    } else
        assert(false && "Constant must be initialized");
    return nullptr;
}

Value *CminusfBuilder::visit(ASTVarDef &node) {
    Type *var_type;

    if (node.type == TYPE_INT) {
        var_type = INT_T;
    } else if (node.type == TYPE_FLOAT) {
        var_type = FLOAT_T;
    } else {
        assert(false && "Unknown type");
    }

    std::vector<int> array_size;
    for (auto exp = node.array_size.rbegin(); exp != node.array_size.rend();
         exp++) {
        auto const_ptr = dynamic_cast<ConstantInt *>((*exp)->accept(*this));
        assert(const_ptr != nullptr && "Array size must be a constant integer");
        array_size.push_back(const_ptr->get_value());
        assert(const_ptr->get_value() > 0 && "Array size must be positive");
        var_type = ArrayType::get(var_type, const_ptr->get_value());
    }

    array_size = std::vector<int>(array_size.rbegin(), array_size.rend());
    if (node.init_val != nullptr) {
        context.init_val_calc = std::make_shared<InitValCalc>(
            module.get(), builder.get(), array_size,
            node.type == TYPE_INT ? INT_T : FLOAT_T, false,
            node.init_val->is_single_exp);
        node.init_val->accept(*this);

        if (scope.in_global()) {
            scope.push(
                node.id,
                GlobalVariable::create(
                    node.id, module.get(), var_type, false,
                    context.init_val_calc->get_const_value(module.get())),
                false);
        } else {
            auto alloca_inst = builder->create_alloca(var_type);
            scope.push(node.id, alloca_inst, false);
            context.init_val_calc->store_value(module.get(), builder.get(),
                                               alloca_inst);
        }
    } else {
        if (scope.in_global()) {
            scope.push(node.id,
                       GlobalVariable::create(
                           node.id, module.get(), var_type, false,
                           ConstantZero::get(var_type, module.get())),
                       false);
        } else {
            scope.push(node.id, builder->create_alloca(var_type), false);
        }
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTConstInitVal &node) {
    auto &calculator = context.init_val_calc;
    if (calculator->is_single_val()) {
        calculator->single_val = node.const_exp->accept(*this);
        return nullptr;
    }

    for (auto &init_val : node.init_vals) {
        if (init_val->is_single_exp) {
            calculator->vals[calculator->cur_idx] =
                init_val->const_exp->accept(*this);
            calculator->cur_idx++;
        } else {
            int next = -1;
            for (int i = 1; i < (int)calculator->suffix_product.size(); i++) {
                if (calculator->cur_idx % calculator->suffix_product[i] == 0) {
                    next = calculator->cur_idx + calculator->suffix_product[i];
                    break;
                }
            }
            if (next == -1) {
                assert(false && "array size error");
            }
            init_val->accept(*this);
            calculator->cur_idx = next;
        }
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTInitVal &node) {
    auto &calculator = context.init_val_calc;
    if (calculator->is_single_val()) {
        calculator->single_val = node.exp->accept(*this);
        return nullptr;
    }
    for (auto &init_val : node.init_vals) {
        if (init_val->is_single_exp) {
            calculator->vals[calculator->cur_idx] =
                init_val->exp->accept(*this);
            calculator->cur_idx++;
        } else {
            int next = -1;
            for (int i = 1; i < (int)calculator->suffix_product.size(); i++) {
                if (calculator->cur_idx % calculator->suffix_product[i] == 0) {
                    next = calculator->cur_idx + calculator->suffix_product[i];
                    break;
                }
            }
            if (next == -1) {
                assert(false && "array size error");
            }
            init_val->accept(*this);
            calculator->cur_idx = next;
        }
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTFuncDef &node) {
    FunctionType *fun_type;
    Type *ret_type;
    std::vector<Type *> param_types;
    if (node.type == TYPE_INT)
        ret_type = INT_T;
    else if (node.type == TYPE_FLOAT)
        ret_type = FLOAT_T;
    else
        ret_type = VOID_T;

    for (auto &param : node.params) {
        param->accept(*this);
        param_types.push_back(context.type_return);
    }

    fun_type = FunctionType::get(ret_type, param_types);
    auto func = Function::create(fun_type, node.id, module.get());
    scope.push(node.id, func);
    context.func = func;
    auto funcBB = BasicBlock::create(module.get(), "entry", func);
    builder->set_insert_point(funcBB);
    scope.enter();
    std::vector<Value *> args;
    for (auto &arg : func->get_args()) {
        args.push_back(&arg);
    }
    for (int i = 0; i < (int)node.params.size(); i++) {
        auto ptr = builder->create_alloca(param_types[i]);
        builder->create_store(args[i], ptr);
        scope.push(node.params[i]->id, ptr, false);
    }
    context.is_func_block = true;
    node.block->accept(*this);
    if (not builder->get_insert_block()->is_terminated()) {
        if (context.func->get_return_type()->is_void_type())
            builder->create_void_ret();
        else if (context.func->get_return_type()->is_float_type())
            builder->create_ret(CONST_FP(0.));
        else
            builder->create_ret(CONST_INT(0));
    }
    scope.exit();
    return nullptr;
}

Value *CminusfBuilder::visit(ASTBlock &node) {
    bool is_func_block = context.is_func_block;
    context.is_func_block = false;
    if (!is_func_block)
        scope.enter();
    for (auto &decl_or_stmt : node.decls_and_stmts) {
        std::visit([this](auto &&arg) { arg->accept(*this); }, decl_or_stmt);
        if (builder->get_insert_block()->is_terminated())
            break;
    }
    if (!is_func_block)
        scope.exit();
    return nullptr;
}

Value *CminusfBuilder::visit(ASTFParam &node) {
    if (node.type == TYPE_VOID) {
        context.type_return = VOID_T;
        return nullptr;
    }
    Type *var_type;
    if (node.type == TYPE_INT)
        var_type = INT_T;
    else if (node.type == TYPE_FLOAT)
        var_type = FLOAT_T;
    else
        assert(false && "Unknown type");

    for (auto dim = node.array_size.rbegin(); dim != node.array_size.rend();
         dim++) {
        auto const_val = static_cast<ConstantInt *>((*dim)->accept(*this));
        if (const_val == nullptr)
            assert(false && "Array size must be a constant integer");
        if (const_val->get_value() <= 0)
            assert(false && "Array size must be positive");

        var_type = ArrayType::get(var_type, const_val->get_value());
    }

    if (node.is_array) {
        var_type = PointerType::get(var_type);
    }

    context.type_return = var_type;
    return nullptr;
}

Value *CminusfBuilder::visit(ASTExpStmt &node) {
    if (!node.is_empty) {
        node.exp->accept(*this);
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTSelectionStmt &node) {
    auto condBB = BasicBlock::create(module.get(), "", context.func);
    auto trueBB = BasicBlock::create(module.get(), "", context.func);
    auto falseBB = BasicBlock::create(module.get(), "", context.func);
    builder->create_br(condBB);
    auto old_condBB = context.condBB;
    auto old_trueBB = context.trueBB;
    auto old_falseBB = context.falseBB;
    context.condBB = condBB;
    context.trueBB = trueBB;
    context.falseBB = falseBB;

    node.cond->accept(*this);

    if (node.else_stmt != nullptr) {
        auto mergeBB = BasicBlock::create(module.get(), "", context.func);

        builder->set_insert_point(trueBB);
        node.if_stmt->accept(*this);
        if (not builder->get_insert_block()->is_terminated())
            builder->create_br(mergeBB);

        builder->set_insert_point(falseBB);
        node.else_stmt->accept(*this);
        if (not builder->get_insert_block()->is_terminated())
            builder->create_br(mergeBB);

        builder->set_insert_point(mergeBB);
    } else {

        builder->set_insert_point(trueBB);
        node.if_stmt->accept(*this);

        if (not builder->get_insert_block()->is_terminated())
            builder->create_br(falseBB);

        builder->set_insert_point(falseBB);
    }
    context.condBB = old_condBB;
    context.trueBB = old_trueBB;
    context.falseBB = old_falseBB;
    return nullptr;
}

Value *CminusfBuilder::visit(ASTIterationStmt &node) {
    auto condBB = BasicBlock::create(module.get(), "", context.func);
    auto trueBB = BasicBlock::create(module.get(), "", context.func);
    auto falseBB = BasicBlock::create(module.get(), "", context.func);
    auto old_condBB = context.condBB;
    auto old_trueBB = context.trueBB;
    auto old_falseBB = context.falseBB;
    auto old_iteration_endBB = context.iteration_endBB;
    context.condBB = condBB;
    context.trueBB = trueBB;
    context.falseBB = falseBB;
    context.iteration_endBB = falseBB;

    builder->create_br(condBB);

    node.cond->accept(*this);

    builder->set_insert_point(trueBB);
    node.stmt->accept(*this);

    if (not builder->get_insert_block()->is_terminated())
        builder->create_br(condBB);

    builder->set_insert_point(falseBB);
    context.condBB = old_condBB;
    context.trueBB = old_trueBB;
    context.falseBB = old_falseBB;
    context.iteration_endBB = old_iteration_endBB;
    return nullptr;
}

Value *CminusfBuilder::visit(ASTReturnStmt &node) {
    if (!node.is_empty) {
        auto exp_ret = node.exp->accept(*this);
        if (context.func->get_return_type()->is_float_type()) {
            if (not exp_ret->get_type()->is_float_type())
                exp_ret = builder->create_sitofp(exp_ret, FLOAT_T);
        } else if (context.func->get_return_type()->is_integer_type()) {
            if (exp_ret->get_type()->is_float_type())
                exp_ret = builder->create_fptosi(exp_ret, INT_T);
            else if (exp_ret->get_type()->is_int1_type())
                exp_ret = builder->create_zext(exp_ret, INT_T);
        }
        builder->create_ret(exp_ret);
    } else {
        builder->create_void_ret();
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTBreakStmt &node) {
    builder->create_br(context.iteration_endBB);
    return nullptr;
}

Value *CminusfBuilder::visit(ASTContinueStmt &node) {
    builder->create_br(context.condBB);
    return nullptr;
}

Value *CminusfBuilder::visit(ASTAssignStmt &node) {
    context.is_l_value = true;
    auto l_val = node.l_val->accept(*this);
    auto l_val_type = l_val->get_type()->get_pointer_element_type();
    auto exp_ret = node.exp->accept(*this);
    if (l_val_type->is_float_type()) {
        if (not exp_ret->get_type()->is_float_type()) {
            exp_ret = builder->create_sitofp(exp_ret, FLOAT_T);
        }
    } else if (l_val_type->is_integer_type()) {
        if (exp_ret->get_type()->is_float_type())
            exp_ret = builder->create_fptosi(exp_ret, INT_T);
        else if (exp_ret->get_type()->is_int1_type())
            exp_ret = builder->create_zext(exp_ret, INT_T);
    } else
        assert(false && "Unknown type");
    builder->create_store(exp_ret, l_val);
    return nullptr;
}

Value *CminusfBuilder::visit(ASTBlockStmt &node) {
    node.block->accept(*this);
    return nullptr;
}
Value *CminusfBuilder::visit(ASTCond &node) {
    builder->set_insert_point(context.condBB);
    if (node.exp->is_binary_exp() &&
        std::static_pointer_cast<ASTBinaryExp>(node.exp)->is_logic_exp()) {
        node.exp->accept(*this);
        return nullptr;
    }
    auto exp_ret = node.exp->accept(*this);
    if (exp_ret->get_type()->is_float_type())
        exp_ret = builder->create_fcmp_ne(exp_ret, CONST_FP(0.));
    else if (exp_ret->get_type()->is_int32_type())
        exp_ret = builder->create_icmp_ne(exp_ret, CONST_INT(0));
    builder->create_cond_br(exp_ret, context.trueBB, context.falseBB);
    return nullptr;
}

Value *CminusfBuilder::visit(ASTLVal &node) {
    auto [ptr, is_const] = scope.find(node.id);
    if (context.is_const_exp and not is_const)
        assert(false && "The expression is not constant");
    if (context.is_l_value and is_const) {
        assert(false && "Trying to modify a constant value");
    }
    bool is_l_value = context.is_l_value;
    context.is_l_value = false;
    // std::vector<int> array_size;
    std::vector<Value *> array_exp;
    for (auto exp : node.array_exp) {
        auto exp_ret = exp->accept(*this);
        if (exp_ret->get_type()->is_float_type()) {
            if (dynamic_cast<Constant *>(exp_ret) != nullptr) {
                exp_ret = ConstantInt::get(
                    (int)dynamic_cast<ConstantFP *>(exp_ret)->get_value(),
                    module.get());
            }
            exp_ret = builder->create_fptosi(exp_ret, INT_T);
        }
        array_exp.push_back(exp_ret);
    }
    auto res_type = ptr->get_type()->get_pointer_element_type();
    unsigned int dim = 0;
    if(res_type->is_pointer_type()) {
        res_type = res_type->get_pointer_element_type();
        dim = 1;
    }

    while (res_type->is_array_type()) {
        auto array_type = static_cast<ArrayType *>(res_type);
        dim += 1;
        res_type = array_type->get_element_type();
    }
    bool is_passing_array = false;
    if (dim > node.array_exp.size()) {
        if (is_const) {
            assert(false && "Trying to pass \'const int *\' to \'int *\'");
        }
        array_exp.push_back(CONST_INT(0));
        is_passing_array = true;
    }
    if (context.is_const_exp) {
        auto [ptr, _] = const_scope.find(node.id);
        auto const_ptr = dynamic_cast<Constant *>(ptr);
        if (const_ptr == nullptr)
            assert(false && "The expression is not constant.");
        for (int i = 0; i < (int)array_exp.size(); i++) {
            auto const_exp_ptr = dynamic_cast<ConstantInt *>(array_exp[i]);
            if (const_exp_ptr == nullptr)
                assert(false && "Array index here must be a constant integer");
            const_ptr =
                dynamic_cast<ConstantArray *>(const_ptr)->get_element_value(
                    const_exp_ptr->get_value());
            if (const_ptr == nullptr)
                assert(false && "Array size does not match");
        }
        return const_ptr;
    }
    Value *pos_ptr;
    if (ptr->get_type()->get_pointer_element_type()->is_array_type()) {
        array_exp.insert(array_exp.begin(), CONST_INT(0));
        pos_ptr = builder->create_gep(ptr, array_exp);
    } else if (ptr->get_type()->get_pointer_element_type()->is_pointer_type())
        pos_ptr = builder->create_gep(builder->create_load(ptr), array_exp);
    else
        pos_ptr = ptr;
    if (is_l_value or is_passing_array)
        return pos_ptr;
    else
        return builder->create_load(pos_ptr);
}

Value *CminusfBuilder::visit(ASTNumber &node) {
    if (node.type == TYPE_INT) {
        return CONST_INT(std::get<int>(node.value));
    } else if (node.type == TYPE_FLOAT) {
        return CONST_FP(std::get<float>(node.value));
    } else {
        assert(false && "Unknown type");
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTBinaryExp &node) {
    if (node.op == OP_AND) {
        auto new_trueBB = BasicBlock::create(module.get(), "", context.func);
        auto old_trueBB = context.trueBB;
        context.trueBB = new_trueBB;

        auto lhs_ret = node.lhs->accept(*this);
        if (lhs_ret != nullptr) {
            if (!lhs_ret->get_type()->is_integer_type()) {
                assert(false &&
                       "Not integer type does not support and operation");
            } else if (lhs_ret->get_type()->is_int32_type())
                lhs_ret = builder->create_icmp_ne(lhs_ret, CONST_INT(0));
            builder->create_cond_br(lhs_ret, context.trueBB, context.falseBB);
        }

        builder->set_insert_point(new_trueBB);
        context.trueBB = old_trueBB;
        auto rhs_ret = node.rhs->accept(*this);
        if (rhs_ret != nullptr) {
            if (!rhs_ret->get_type()->is_integer_type()) {
                assert(false &&
                       "Not integer type does not support and operation");
            } else if (rhs_ret->get_type()->is_int32_type())
                rhs_ret = builder->create_icmp_ne(rhs_ret, CONST_INT(0));
            builder->create_cond_br(rhs_ret, context.trueBB, context.falseBB);
        }
        return nullptr;
    }
    if (node.op == OP_OR) {
        auto new_falseBB = BasicBlock::create(module.get(), "", context.func);
        auto old_falseBB = context.falseBB;
        context.falseBB = new_falseBB;

        auto lhs_ret = node.lhs->accept(*this);
        if (lhs_ret != nullptr) {
            if (!lhs_ret->get_type()->is_integer_type()) {
                assert(false &&
                       "Not integer type does not support or operation");
            } else if (lhs_ret->get_type()->is_int32_type())
                lhs_ret = builder->create_icmp_ne(lhs_ret, CONST_INT(0));
            builder->create_cond_br(lhs_ret, context.trueBB, context.falseBB);
        }

        builder->set_insert_point(new_falseBB);
        context.falseBB = old_falseBB;
        auto rhs_ret = node.rhs->accept(*this);
        if (rhs_ret != nullptr) {
            if (!rhs_ret->get_type()->is_integer_type()) {
                assert(false &&
                       "Not integer type does not support or operation");
            } else if (rhs_ret->get_type()->is_int32_type())
                rhs_ret = builder->create_icmp_ne(rhs_ret, CONST_INT(0));
            builder->create_cond_br(rhs_ret, context.trueBB, context.falseBB);
        }
        return nullptr;
    }
    auto lhs = node.lhs->accept(*this);
    auto rhs = node.rhs->accept(*this);

    if (dynamic_cast<Constant *>(lhs) != nullptr and
        dynamic_cast<Constant *>(rhs) != nullptr) {
        if (dynamic_cast<ConstantFP *>(lhs) != nullptr or
            dynamic_cast<ConstantFP *>(rhs) != nullptr) {
            float lhs_val, rhs_val;
            if (dynamic_cast<ConstantFP *>(lhs) != nullptr)
                lhs_val = dynamic_cast<ConstantFP *>(lhs)->get_value();
            else if (dynamic_cast<ConstantInt *>(lhs) != nullptr)
                lhs_val = dynamic_cast<ConstantInt *>(lhs)->get_value();
            else
                assert(false && "Unknown type");
            if (dynamic_cast<ConstantFP *>(rhs) != nullptr)
                rhs_val = dynamic_cast<ConstantFP *>(rhs)->get_value();
            else if (dynamic_cast<ConstantInt *>(rhs) != nullptr)
                rhs_val = dynamic_cast<ConstantInt *>(rhs)->get_value();
            else
                assert(false && "Unknown type");
            switch (node.op) {
            case OP_PLUS:
                return CONST_FP(lhs_val + rhs_val);
            case OP_MINUS:
                return CONST_FP(lhs_val - rhs_val);
            case OP_MUL:
                return CONST_FP(lhs_val * rhs_val);
            case OP_DIV:
                return CONST_FP(lhs_val / rhs_val);
            case OP_MOD:
                assert(false && "Float type does not support mod operation");
                return nullptr;
            case OP_LT:
                return CONST_INT(lhs_val < rhs_val);
            case OP_LE:
                return CONST_INT(lhs_val <= rhs_val);
            case OP_GT:
                return CONST_INT(lhs_val > rhs_val);
            case OP_GE:
                return CONST_INT(lhs_val >= rhs_val);
            case OP_EQ:
                return CONST_INT(lhs_val == rhs_val);
            case OP_NEQ:
                return CONST_INT(lhs_val != rhs_val);
            default:
                assert(false && "Unknown operator");
            }
        } else {
            int lhs_val, rhs_val;
            if (dynamic_cast<ConstantInt *>(lhs) != nullptr)
                lhs_val = dynamic_cast<ConstantInt *>(lhs)->get_value();
            else
                assert(false && "Unknown type");
            if (dynamic_cast<ConstantInt *>(rhs) != nullptr)
                rhs_val = dynamic_cast<ConstantInt *>(rhs)->get_value();
            else
                assert(false && "Unknown type");
            switch (node.op) {
            case OP_PLUS:
                return CONST_INT(lhs_val + rhs_val);
            case OP_MINUS:
                return CONST_INT(lhs_val - rhs_val);
            case OP_MUL:
                return CONST_INT(lhs_val * rhs_val);
            case OP_DIV:
                return CONST_INT(lhs_val / rhs_val);
            case OP_MOD:
                return CONST_INT(lhs_val % rhs_val);
            case OP_LT:
                return CONST_INT(lhs_val < rhs_val);
            case OP_LE:
                return CONST_INT(lhs_val <= rhs_val);
            case OP_GT:
                return CONST_INT(lhs_val > rhs_val);
            case OP_GE:
                return CONST_INT(lhs_val >= rhs_val);
            case OP_EQ:
                return CONST_INT(lhs_val == rhs_val);
            case OP_NEQ:
                return CONST_INT(lhs_val != rhs_val);
            default:
                assert(false && "Unknown operator");
            }
        }
    } else if (lhs->get_type()->is_float_type() or
               rhs->get_type()->is_float_type()) {
        if (context.is_const_exp) {
            assert(false && "The expression is not constant.");
        }
        if (not lhs->get_type()->is_float_type())
            lhs = builder->create_sitofp(lhs, FLOAT_T);
        if (not rhs->get_type()->is_float_type())
            rhs = builder->create_sitofp(rhs, FLOAT_T);
        switch (node.op) {
        case OP_PLUS:
            return builder->create_fadd(lhs, rhs);
        case OP_MINUS:
            return builder->create_fsub(lhs, rhs);
        case OP_MUL:
            return builder->create_fmul(lhs, rhs);
        case OP_DIV:
            return builder->create_fdiv(lhs, rhs);
        case OP_MOD:
            assert(false && "Float type does not support mod operation");
            return nullptr;
        case OP_LT:
            return builder->create_fcmp_lt(lhs, rhs);
        case OP_LE:
            return builder->create_fcmp_le(lhs, rhs);
        case OP_GT:
            return builder->create_fcmp_gt(lhs, rhs);
        case OP_GE:
            return builder->create_fcmp_ge(lhs, rhs);
        case OP_EQ:
            return builder->create_fcmp_eq(lhs, rhs);
        case OP_NEQ:
            return builder->create_fcmp_ne(lhs, rhs);
        default:
            assert(false && "Unknown operator");
        }
    } else {
        if (context.is_const_exp) {
            assert(false && "The expression is not constant.");
        }
        if (lhs->get_type()->is_int1_type())
            lhs = builder->create_zext(lhs, INT_T);
        if (rhs->get_type()->is_int1_type())
            rhs = builder->create_zext(rhs, INT_T);
        std::cerr<<lhs->get_type()->is_pointer_type()<<std::endl;
        std::cerr<<rhs->get_type()->is_pointer_type()<<std::endl;
        switch (node.op) {
        case OP_PLUS:
            return builder->create_iadd(lhs, rhs);
        case OP_MINUS:
            return builder->create_isub(lhs, rhs);
        case OP_MUL:
            return builder->create_imul(lhs, rhs);
        case OP_DIV:
            return builder->create_isdiv(lhs, rhs);
        case OP_MOD:
            return builder->create_isrem(lhs, rhs);
        case OP_LT:
            return builder->create_icmp_lt(lhs, rhs);
        case OP_LE:
            return builder->create_icmp_le(lhs, rhs);
        case OP_GT:
            return builder->create_icmp_gt(lhs, rhs);
        case OP_GE:
            return builder->create_icmp_ge(lhs, rhs);
        case OP_EQ:
            return builder->create_icmp_eq(lhs, rhs);
        case OP_NEQ:
            return builder->create_icmp_ne(lhs, rhs);
        default:
            assert(false && "Unknown operator");
        }
    }
    assert(false && "Unknown operator");
    return nullptr;
}

Value *CminusfBuilder::visit(ASTUnaryExp &node) {
    if (node.has_unary_op) {
        auto exp_ret = node.exp->accept(*this);
        if (dynamic_cast<Constant *>(exp_ret) != nullptr) {
            if (dynamic_cast<ConstantFP *>(exp_ret) != nullptr) {
                float val = dynamic_cast<ConstantFP *>(exp_ret)->get_value();
                switch (node.op) {
                case (OP_POS):
                    return CONST_FP(val);
                case OP_NEG:
                    return CONST_FP(-val);
                case OP_NOT:
                    return CONST_INT(!val);
                default:
                    assert(false && "Unknown operator");
                }
            } else {
                int val = dynamic_cast<ConstantInt *>(exp_ret)->get_value();
                switch (node.op) {
                case (OP_POS):
                    return CONST_INT(val);
                case OP_NEG:
                    return CONST_INT(-val);
                case OP_NOT:
                    return CONST_INT(!val);
                default:
                    assert(false && "Unknown operator");
                }
            }
        } else {
            if (context.is_const_exp) {
                assert(false && "The expression is not constant.");
            }
            if (exp_ret->get_type()->is_float_type()) {
                switch (node.op) {
                case OP_POS:
                    return exp_ret;
                case OP_NEG:
                    return builder->create_fsub(CONST_INT(0), exp_ret);
                case OP_NOT:
                    exp_ret = builder->create_fcmp_eq(exp_ret, CONST_FP(0.));
                default:
                    assert(false && "Unknown operator");
                }
            } else {
                if (exp_ret->get_type()->is_int1_type())
                    exp_ret = builder->create_zext(exp_ret, INT_T);
                switch (node.op) {
                case (OP_POS):
                    return exp_ret;
                case OP_NEG:
                    return builder->create_isub(CONST_INT(0), exp_ret);
                case OP_NOT:
                    return builder->create_icmp_eq(CONST_INT(0), exp_ret);
                default:
                    assert(false && "Unknown operator");
                }
            }
        }
    } else if (node.is_l_val()) {
        return node.l_val->accept(*this);
    } else if (node.is_number()) {
        return node.number->accept(*this);
    } else if (node.is_func_call()) {
        std::vector<Value *> args;
        for (auto &arg : node.func_call_args) {
            args.push_back(arg->accept(*this));
        }
        auto [func, _] = scope.find(node.func_call_id);
        auto func_type = static_cast<FunctionType *>(func->get_type());
        if (func_type->get_num_of_args() != args.size()) {
            assert(false &&
                   "The number of arguments is not equal to the number of "
                   "parameters.");
        }
        for (unsigned int i = 0; i < args.size(); i++) {
            if (func_type->get_param_type(i)->is_float_type() and
                not args[i]->get_type()->is_float_type())
                args[i] = builder->create_sitofp(args[i], FLOAT_T);
            else if (func_type->get_param_type(i)->is_integer_type()) {
                if (args[i]->get_type()->is_int1_type())
                    args[i] = builder->create_zext(args[i], INT_T);
                else if (args[i]->get_type()->is_float_type())
                    args[i] = builder->create_fptosi(args[i], INT_T);
            }
        }
        return builder->create_call(func, args);
    }

    assert(false && "Unknown unary expression");
    return nullptr;
}

Value *CminusfBuilder::visit(ASTConstExp &node) {
    context.is_const_exp = true;
    auto ret = node.exp->accept(*this);
    context.is_const_exp = false;
    return ret;
}