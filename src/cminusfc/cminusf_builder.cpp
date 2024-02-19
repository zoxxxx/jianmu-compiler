#include "Constant.hpp"
#include "Function.hpp"
#include "GlobalVariable.hpp"
#include "Instruction.hpp"
#include "Type.hpp"
#include "Value.hpp"
#include "ast.hpp"
#include "cminusf_builder.hpp"
#include "logging.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <memory>

#define CONST_FP(num) ConstantFP::get((float)num, module.get())
#define CONST_INT(num) ConstantInt::get(num, module.get())

// types
Type *VOID_T;
Type *INT_T;
Type *INTPTR_T;
Type *FLOAT_T;
Type *FLOATPTR_T;

/*
 * use CMinusfBuilder::Scope to construct scopes
 * scope.enter: enter a new scope
 * scope.exit: exit current scope
 * scope.push: add a new binding to current scope
 * scope.find: find and return the value bound to the name
 */

void CminusfBuilder::store_array(Value *alloca_inst, std::vector<Value *> idxs,
                                 std::vector<ConstantInt *> array_size,
                                 ConstantArray *vals) {
    if (idxs.size() == array_size.size()) {
        idxs.insert(idxs.begin(), CONST_INT(0));
        auto ptr = builder->create_gep(alloca_inst, idxs);
        Constant *val = vals;
        for (int i = 0; i < (int)idxs.size(); i++) {
            auto array_ptr = dynamic_cast<ConstantArray *>(val);
            assert(array_ptr != nullptr && "Array size not match");
            val = array_ptr->get_element_value(
                (dynamic_cast<ConstantInt *>(idxs[i]))->get_value());
        }
        builder->create_store(ptr, val);
        return;
    }
    for (int i = 0; i < array_size[idxs.size()]->get_value(); i++) {
        idxs.push_back(CONST_INT(i));
        store_array(alloca_inst, idxs, array_size, vals);
        idxs.pop_back();
    }
}
Value *CminusfBuilder::visit(ASTProgram &node) {
    VOID_T = module->get_void_type();
    INT_T = module->get_int32_type();
    INTPTR_T = module->get_int32_ptr_type();
    FLOAT_T = module->get_float_type();
    FLOATPTR_T = module->get_float_ptr_type();
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
    Type *var_type = node.type == TYPE_INT ? INT_T : FLOAT_T;

    context.is_calc_array_dem = true;
    context.array_size.clear();
    for (auto &exp : node.array_size) {
        auto const_ptr = dynamic_cast<ConstantInt *>(exp->accept(*this));
        assert(const_ptr != nullptr && "Array size must be a constant integer");
        context.array_size.push_back(const_ptr);
        assert(const_ptr->get_value() > 0 && "Array size must be positive");
        var_type = ArrayType::get(var_type, const_ptr->get_value());
    }
    context.is_calc_array_dem = false;
    if (node.init_val != nullptr) {
        context.is_calc_init_val = true;
        auto init_val = dynamic_cast<Constant *>(node.init_val->accept(*this));
        context.is_calc_init_val = false;
        if (scope.in_global()) {
            scope.push(node.id,
                       GlobalVariable::create(node.id, module.get(), var_type,
                                              true, init_val),
                       true);
        } else {
            auto alloca_inst = builder->create_alloca(var_type);
            scope.push(node.id, alloca_inst, true);
            auto init_val_array_ptr = dynamic_cast<ConstantArray *>(init_val);
            if (context.array_size.size() == 0) {
                builder->create_store(alloca_inst, init_val);
            } else {
                store_array(alloca_inst, {}, context.array_size,
                            init_val_array_ptr);
            }
        }
    } else {
        if (scope.in_global()) {
            scope.push(node.id,
                       GlobalVariable::create(
                           node.id, module.get(), var_type, true,
                           ConstantZero::get(var_type, module.get())),
                       true);
        } else {
            scope.push(node.id, builder->create_alloca(var_type), true);
        }
    }
    return nullptr;
}
Value *CminusfBuilder::visit(ASTBtype &node) {
    Value *nodeS;
    nodeS.DownType = node.type;
    return nodeS;
}
Value *CminusfBuilder::visit(ASTConstDefList &node) {
    if (node.type == 1) {
        (node.Vec[0])->accept(*this);
    } else {
        (node.Vec[0])->accept(*this);
        (node.Vec[1])->accept(*this);
    }
}
Value *CminusfBuilder::visit(ASTConstDef &node) {
    Type *var_type = nullptr;
    if (context.vartype == 1) // int
        var_type = INT_T;
    else if (context.vartype == 2) // float
        var_type = FLOAT_T;
    Value *ConstExpListS = (node.Vec[1])->accept(*this);
    Value *ConstInitValS = (node.Vec[2])->accept(*this);
    if (ConstExpListS.is_empty == 1) { // not array
        if (scope.in_global()) {
            scope.push(node.id,
                       GlobalVariable::create(
                           node.id, module.get(), var_type, true,
                           ConstInitValS.Data)); // global初始化会自带store
        } else {
            auto ptr = builder->create_alloca(var_type);
            builder->create_store(ptr, ConstInitValS.Data);
            scope.push(node.id, ptr);
        }
    } else { // array
        if (scope.in_global()) {

        } else {
        }
    }
}
Value *CminusfBuilder::visit(ASTIdent &);
Value *CminusfBuilder::visit(ASTConstExpList &);
Value *CminusfBuilder::visit(ASTConstInitVal &);
Value *CminusfBuilder::visit(ASTConstExp &);
Value *CminusfBuilder::visit(ASTConstInitValList &);

Value *CminusfBuilder::visit(ASTVarDefList &);
Value *CminusfBuilder::visit(ASTVarDef &);
Value *CminusfBuilder::visit(ASTExpList &);
Value *CminusfBuilder::visit(ASTInitVal &);
Value *CminusfBuilder::visit(ASTExp &);
Value *CminusfBuilder::visit(ASTInitValList &);

Value *CminusfBuilder::visit(ASTFuncType &);
Value *CminusfBuilder::visit(ASTFuncFParams &);
Value *CminusfBuilder::visit(ASTBlock &);
Value *CminusfBuilder::visit(ASTFuncFParam &);
Value *CminusfBuilder::visit(ASTBlockItemList &);
Value *CminusfBuilder::visit(ASTBlockItem &);

Value *CminusfBuilder::visit(ASTLVal &);
Value *CminusfBuilder::visit(ASTCond &);

Value *CminusfBuilder::visit(ASTAddExp &);
Value *CminusfBuilder::visit(ASTLOrExp &);

Value *CminusfBuilder::visit(ASTLOrExp &);
Value *CminusfBuilder::visit(ASTPrimaryExp &node) {
    return (node.Vec[0])->accept(*this);
}
Value *CminusfBuilder::visit(ASTNumber &node) {
    return (node.Vec[0])->accept(*this);
}
Value *CminusfBuilder::visit(ASTIntConst &node) {
    Value *now;
    now.Data = CONST_INT(node.num);
    context.RConstI = node.num;
    return now;
}
Value *CminusfBuilder::visit(ASTFloatConst &node) {
    Value *now;
    now.Data = CONST_FP(node.num);
    context.RConstF = node.num;
    return now;
}
Value *CminusfBuilder::visit(ASTUnaryExp &node) {
    if (node.Type == 3 || node.Type == 2) {
        std::vector<Value *> args;
        for (auto &arg : node.args) {
            args.push_back(arg->accept(*this));
        }
        auto func = scope.find(node.id);
        auto func_type = static_cast<FunctionType *>(func->get_type());
        for (unsigned int i = 0; i < args.size(); i++) {
            if (func_type->get_param_type(i)->is_float_type() and
                not args[i]->get_type()->is_float_type())
                args[i] = builder->create_sitofp(args[i], FLOAT_T);
            else if (func_type->get_param_type(i)->is_integer_type()) {
                if (args[i]->get_type()->is_float_type())
                    args[i] = builder->create_fptosi(args[i], INT32_T);
            }
        }
        return builder->create_call(func, args);
    } else if (node.Type == 1)
        return (node.Vec[0])->accept(*this);
    else if (node.Type == 4) {
        if (node.UnaryOp == '+') {
            return (node.Vec[0])->accept(*this);
        } else if (node.UnaryOp == '-') {
            auto qq = (node.Vec[0])->accept(*this);
            if (qq.Data->get_type()->is_integer_type()) {
                qq.Data = builder->create_isub(0, qq.Data);
                context.RConstI *= -1;
            } else {
                qq.Data = builder->create_fsub(0, qq.Data);
                context.RConstF *= -1;
            }
            return qq;
        } else if (node.UnaryOp == '!') {
            auto qq = (node.Vec[0])->accept(*this);
            if (qq.Data->get_type()->is_integer_type()) {
                qq.Data = builder->create_icmp_eq(0, qq.Data);
                context.RConstI = !(context.RConstI);
            } else {
                qq.Data = builder->create_fcmp_eq(0, qq.Data);
                context.RConstF *= -1;
            }
            return qq;
        }
    }
}

Value *CminusfBuilder::visit(ASTMulExp &node) {
    if (node.Type == 1) {
        return (node.Vec[0])->accept(*this);
    } else {
        auto p1 = (node.Vec[0])->accept(*this);
        auto p2 = (node.Vec[1])->accept(*this);
        if ((p1.Data)->get_type()->is_float_type() or
            (p2.Data)->get_type()->is_float_type()) {
            if (not(p1.Data)->get_type()->is_float_type())
                p1.Data = builder->create_sitofp(p1.Data, FLOAT_T);
            if (not(p2.Data)->get_type()->is_float_type())
                p2.Data = builder->create_sitofp(p2.Data, FLOAT_T);
            if (node.op == '*') {

                p1.Data = builder->create_fmul(p1.Data, p2.Data);
                context.RConstF =
            } else if (node.op == '/') {
                return builder->create_fdiv(lhs, rhs);
            }
        } else {
            if (lhs->get_type()->is_int1_type())
                lhs = builder->create_zext(lhs, INT32_T);
            if (rhs->get_type()->is_int1_type())
                rhs = builder->create_zext(rhs, INT32_T);
            if (node.op == OP_MUL) {
                return builder->create_imul(lhs, rhs);
            } else if (node.op == OP_DIV) {
                return builder->create_isdiv(lhs, rhs);
            }
        }
    }
}
Value *CminusfBuilder::visit(ASTRelExp &node) {}
Value *CminusfBuilder::visit(ASTEqExp &node);
Value *CminusfBuilder::visit(ASTLAndExp &node);

Value *CminusfBuilder::visit(ASTNum &node) {
    if (node.type == TYPE_INT)
        return CONST_INT(node.i_val);
    else if (node.type == TYPE_FLOAT)
        return CONST_FP(node.f_val);
    else
        LOG(ERROR) << "Unknown type.";
    return nullptr;
}

Value *CminusfBuilder::visit(ASTVarDeclaration &node) {
    if (node.num == nullptr) {
        if (scope.in_global()) {
            auto initializer = ConstantZero::get(var_type, module.get());
            scope.push(node.id,
                       GlobalVariable::create(node.id, module.get(), var_type,
                                              false, initializer));
        } else {
            scope.push(node.id, builder->create_alloca(var_type));
        }
    } else {
        auto const_val = static_cast<ConstantInt *>(node.num->accept(*this));
        auto array_type = ArrayType::get(var_type, const_val->get_value());
        if (scope.in_global()) {
            auto initializer = ConstantZero::get(var_type, module.get());
            scope.push(node.id,
                       GlobalVariable::create(node.id, module.get(), array_type,
                                              false, initializer));
        } else {
            scope.push(node.id, builder->create_alloca(array_type));
        }
    }

    return nullptr;
}

Value *CminusfBuilder::visit(ASTFunDeclaration &node) {
    FunctionType *fun_type;
    Type *ret_type;
    std::vector<Type *> param_types;
    if (node.type == TYPE_INT)
        ret_type = INT32_T;
    else if (node.type == TYPE_FLOAT)
        ret_type = FLOAT_T;
    else
        ret_type = VOID_T;

    for (auto &param : node.params) {
        // Please accomplish param_types.
        if (param->type == TYPE_INT) {
            if (param->isarray)
                param_types.push_back(INT32PTR_T);
            else
                param_types.push_back(INT32_T);
        } else if (param->type == TYPE_FLOAT) {
            if (param->isarray)
                param_types.push_back(FLOATPTR_T);
            else
                param_types.push_back(FLOAT_T);
        } else
            param_types.push_back(VOID_T);
    }

    fun_type = FunctionType::get(ret_type, param_types);
    auto func = Function::create(fun_type, node.id, module.get());
    scope.push(node.id, func);
    context.func = func;
    auto funBB = BasicBlock::create(module.get(), "entry", func);
    builder->set_insert_point(funBB);
    scope.enter();
    std::vector<Value *> args;
    for (auto &arg : func->get_args()) {
        args.push_back(&arg);
    }
    for (int i = 0; i < node.params.size(); ++i) {
        // You need to deal with params and store them in the scope.
        auto ptr = builder->create_alloca(param_types[i]);
        builder->create_store(args[i], ptr);
        scope.push(node.params[i]->id, ptr);
    }
    node.compound_stmt->accept(*this);
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

Value *CminusfBuilder::visit(ASTParam &node) {
    // This function is empty now.
    // don't know what to do here
    return nullptr;
}

Value *CminusfBuilder::visit(ASTCompoundStmt &node) {
    // to deal with complex statements.

    scope.enter();
    // FIXME: There will be a bug if the function has a local variable named the
    // same as a function parameter. but it seems the code doesn't care about
    // the repeated name.
    for (auto &decl : node.local_declarations) {
        decl->accept(*this);
    }

    for (auto &stmt : node.statement_list) {
        stmt->accept(*this);
        if (builder->get_insert_block()->is_terminated())
            break;
    }
    scope.exit();
    return nullptr;
}

Value *CminusfBuilder::visit(ASTExpressionStmt &node) {
    node.expression->accept(*this);
    return nullptr;
}

Value *CminusfBuilder::visit(ASTSelectionStmt &node) {
    auto expression_ret = node.expression->accept(*this);
    if (expression_ret->get_type()->is_float_type())
        expression_ret = builder->create_fcmp_ne(expression_ret, CONST_FP(0.));
    else if (expression_ret->get_type()->is_int32_type())
        expression_ret = builder->create_icmp_ne(expression_ret, CONST_INT(0));
    if (node.else_statement != nullptr) {
        auto trueBB = BasicBlock::create(module.get(), "", context.func);
        auto falseBB = BasicBlock::create(module.get(), "", context.func);
        auto mergeBB = BasicBlock::create(module.get(), "", context.func);
        builder->create_cond_br(expression_ret, trueBB, falseBB);

        builder->set_insert_point(trueBB);
        node.if_statement->accept(*this);
        if (not builder->get_insert_block()->is_terminated())
            builder->create_br(mergeBB);

        builder->set_insert_point(falseBB);
        node.else_statement->accept(*this);
        if (not builder->get_insert_block()->is_terminated())
            builder->create_br(mergeBB);

        builder->set_insert_point(mergeBB);
    } else {
        auto trueBB = BasicBlock::create(module.get(), "", context.func);
        auto mergeBB = BasicBlock::create(module.get(), "", context.func);

        builder->create_cond_br(expression_ret, trueBB, mergeBB);

        builder->set_insert_point(trueBB);
        node.if_statement->accept(*this);

        if (not builder->get_insert_block()->is_terminated())
            builder->create_br(mergeBB);

        builder->set_insert_point(mergeBB);
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTIterationStmt &node) {
    auto condBB = BasicBlock::create(module.get(), "", context.func);
    auto bodyBB = BasicBlock::create(module.get(), "", context.func);
    auto mergeBB = BasicBlock::create(module.get(), "", context.func);

    builder->create_br(condBB);

    builder->set_insert_point(condBB);
    auto expression_ret = node.expression->accept(*this);
    if (expression_ret->get_type()->is_float_type())
        expression_ret = builder->create_fcmp_ne(expression_ret, CONST_FP(0.));
    else if (expression_ret->get_type()->is_int32_type())
        expression_ret = builder->create_icmp_ne(expression_ret, CONST_INT(0));
    builder->create_cond_br(expression_ret, bodyBB, mergeBB);

    builder->set_insert_point(bodyBB);
    node.statement->accept(*this);
    if (not builder->get_insert_block()->is_terminated())
        builder->create_br(condBB);

    builder->set_insert_point(mergeBB);
    return nullptr;
}

Value *CminusfBuilder::visit(ASTReturnStmt &node) {
    if (node.expression == nullptr) {
        builder->create_void_ret();
        return nullptr;
    } else {
        // You need to solve other return cases (e.g. return an integer).
        auto expression_ret = node.expression->accept(*this);
        if (context.func->get_return_type()->is_float_type()) {
            if (not expression_ret->get_type()->is_float_type())
                expression_ret =
                    builder->create_sitofp(expression_ret, FLOAT_T);
        } else if (context.func->get_return_type()->is_integer_type()) {
            if (expression_ret->get_type()->is_float_type())
                expression_ret =
                    builder->create_fptosi(expression_ret, INT32_T);
            else if (expression_ret->get_type()->is_int1_type())
                expression_ret = builder->create_zext(expression_ret, INT32_T);
        }
        builder->create_ret(expression_ret);
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTVar &node) {
    auto isLValue = context.isLValue;
    context.isLValue = false;
    if (node.expression == nullptr) {
        auto ptr = scope.find(node.id);
        if (isLValue)
            return ptr;
        else if (ptr->get_type()->get_pointer_element_type()->is_array_type())
            // if var is pointer to array, then we need to get the pointer to
            // the first element of the array as this will only happen when we
            // are trying to pass an array as a parameter
            return builder->create_gep(ptr, {CONST_INT(0), CONST_INT(0)});
        else
            return builder->create_load(ptr);
    } else {
        auto expression_ret = node.expression->accept(*this);
        if (not expression_ret->get_type()->is_integer_type()) {
            expression_ret = builder->create_fptosi(expression_ret, INT32_T);
        }

        auto exceptBB = BasicBlock::create(module.get(), "", context.func);
        auto mergeBB = BasicBlock::create(module.get(), "", context.func);

        auto icmp = builder->create_icmp_lt(expression_ret, CONST_INT(0));
        builder->create_cond_br(icmp, exceptBB, mergeBB);

        builder->set_insert_point(exceptBB);
        builder->create_call(scope.find("neg_idx_except"), {});
        if (not builder->get_insert_block()->is_terminated())
            builder->create_br(mergeBB);

        builder->set_insert_point(mergeBB);
        auto var = scope.find(node.id);
        GetElementPtrInst *ptr;

        if (var->get_type()->get_pointer_element_type()->is_array_type())
            // if var is pointer to array, then we need to get the pointer to
            // the first element of the array
            ptr = builder->create_gep(var, {CONST_INT(0), expression_ret});
        else {
            // if var is a pointer to pointer(which is the start of an array)
            // then we need to get the pointer to the first element of the array
            auto ptr_res = builder->create_load(var);
            ptr = builder->create_gep(ptr_res, {expression_ret});
        }
        if (isLValue)
            return ptr;
        else
            return builder->create_load(ptr);
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTAssignExpression &node) {
    context.isLValue = true;
    auto var = node.var->accept(*this);
    auto var_type = var->get_type()->get_pointer_element_type();
    auto expression_ret = node.expression->accept(*this);
    if (var_type->is_float_type()) {
        if (not expression_ret->get_type()->is_float_type()) {
            expression_ret = builder->create_sitofp(expression_ret, FLOAT_T);
        }
    } else if (var_type->is_integer_type()) {
        if (expression_ret->get_type()->is_float_type())
            expression_ret = builder->create_fptosi(expression_ret, INT32_T);
        else if (expression_ret->get_type()->is_int1_type())
            expression_ret = builder->create_zext(expression_ret, INT32_T);
    }
    builder->create_store(expression_ret, var);
    return expression_ret;
}

Value *CminusfBuilder::visit(ASTSimpleExpression &node) {
    if (node.additive_expression_r == nullptr) {
        return node.additive_expression_l->accept(*this);
    } else {
        auto lhs = node.additive_expression_l->accept(*this);
        auto rhs = node.additive_expression_r->accept(*this);
        if (lhs->get_type()->is_float_type() or
            rhs->get_type()->is_float_type()) {
            if (not lhs->get_type()->is_float_type())
                lhs = builder->create_sitofp(lhs, FLOAT_T);
            if (not rhs->get_type()->is_float_type())
                rhs = builder->create_sitofp(rhs, FLOAT_T);
            if (node.op == OP_LT) {
                return builder->create_fcmp_lt(lhs, rhs);
            } else if (node.op == OP_LE) {
                return builder->create_fcmp_le(lhs, rhs);
            } else if (node.op == OP_GT) {
                return builder->create_fcmp_gt(lhs, rhs);
            } else if (node.op == OP_GE) {
                return builder->create_fcmp_ge(lhs, rhs);
            } else if (node.op == OP_EQ) {
                return builder->create_fcmp_eq(lhs, rhs);
            } else if (node.op == OP_NEQ) {
                return builder->create_fcmp_ne(lhs, rhs);
            }
        } else {
            if (lhs->get_type()->is_int1_type())
                lhs = builder->create_zext(lhs, INT32_T);
            if (rhs->get_type()->is_int1_type())
                rhs = builder->create_zext(rhs, INT32_T);
            if (node.op == OP_LT) {
                return builder->create_icmp_lt(lhs, rhs);
            } else if (node.op == OP_LE) {
                return builder->create_icmp_le(lhs, rhs);
            } else if (node.op == OP_GT) {
                return builder->create_icmp_gt(lhs, rhs);
            } else if (node.op == OP_GE) {
                return builder->create_icmp_ge(lhs, rhs);
            } else if (node.op == OP_EQ) {
                return builder->create_icmp_eq(lhs, rhs);
            } else if (node.op == OP_NEQ) {
                return builder->create_icmp_ne(lhs, rhs);
            }
        }
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTAdditiveExpression &node) {
    if (node.additive_expression == nullptr) {
        return node.term->accept(*this);
    } else {
        auto lhs = node.additive_expression->accept(*this);
        auto rhs = node.term->accept(*this);
        if (lhs->get_type()->is_float_type() or
            rhs->get_type()->is_float_type()) {
            if (not lhs->get_type()->is_float_type())
                lhs = builder->create_sitofp(lhs, FLOAT_T);
            if (not rhs->get_type()->is_float_type())
                rhs = builder->create_sitofp(rhs, FLOAT_T);
            if (node.op == OP_PLUS) {
                return builder->create_fadd(lhs, rhs);
            } else if (node.op == OP_MINUS) {
                return builder->create_fsub(lhs, rhs);
            }
        } else {
            if (lhs->get_type()->is_int1_type())
                lhs = builder->create_zext(lhs, INT32_T);
            if (rhs->get_type()->is_int1_type())
                rhs = builder->create_zext(rhs, INT32_T);
            if (node.op == OP_PLUS) {
                return builder->create_iadd(lhs, rhs);
            } else if (node.op == OP_MINUS) {
                return builder->create_isub(lhs, rhs);
            }
        }
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTTerm &node) {
    if (node.term == nullptr) {
        return node.factor->accept(*this);
    } else {
        auto lhs = node.term->accept(*this);
        auto rhs = node.factor->accept(*this);
        if (lhs->get_type()->is_float_type() or
            rhs->get_type()->is_float_type()) {
            if (not lhs->get_type()->is_float_type())
                lhs = builder->create_sitofp(lhs, FLOAT_T);
            if (not rhs->get_type()->is_float_type())
                rhs = builder->create_sitofp(rhs, FLOAT_T);
            if (node.op == OP_MUL) {
                return builder->create_fmul(lhs, rhs);
            } else if (node.op == OP_DIV) {
                return builder->create_fdiv(lhs, rhs);
            }
        } else {
            if (lhs->get_type()->is_int1_type())
                lhs = builder->create_zext(lhs, INT32_T);
            if (rhs->get_type()->is_int1_type())
                rhs = builder->create_zext(rhs, INT32_T);
            if (node.op == OP_MUL) {
                return builder->create_imul(lhs, rhs);
            } else if (node.op == OP_DIV) {
                return builder->create_isdiv(lhs, rhs);
            }
        }
    }
    return nullptr;
}

Value *CminusfBuilder::visit(ASTCall &node) {
    std::vector<Value *> args;
    for (auto &arg : node.args) {
        args.push_back(arg->accept(*this));
    }
    auto func = scope.find(node.id);
    auto func_type = static_cast<FunctionType *>(func->get_type());
    if (func_type->get_num_of_args() != args.size()) {
        LOG(ERROR) << "The number of arguments is not equal to the number of "
                      "parameters.";
    }
    for (unsigned int i = 0; i < args.size(); i++) {
        if (func_type->get_param_type(i)->is_float_type() and
            not args[i]->get_type()->is_float_type())
            args[i] = builder->create_sitofp(args[i], FLOAT_T);
        else if (func_type->get_param_type(i)->is_integer_type()) {
            if (args[i]->get_type()->is_int1_type())
                args[i] = builder->create_zext(args[i], INT32_T);
            else if (args[i]->get_type()->is_float_type())
                args[i] = builder->create_fptosi(args[i], INT32_T);
        }
        LOG(DEBUG) << func_type->get_param_type(i)->print();
        LOG(DEBUG) << args[i]->get_type()->print();
    }
    return builder->create_call(func, args);
}
