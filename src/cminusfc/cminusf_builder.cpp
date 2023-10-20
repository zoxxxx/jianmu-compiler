#include "cminusf_builder.hpp"
#include "Constant.hpp"
#include "Function.hpp"
#include "GlobalVariable.hpp"
#include "Instruction.hpp"
#include "Type.hpp"
#include "ast.hpp"
#include "logging.hpp"
#include <algorithm>

#define CONST_FP(num) ConstantFP::get((float)num, module.get())
#define CONST_INT(num) ConstantInt::get(num, module.get())

// types
Type *VOID_T;
Type *INT1_T;
Type *INT32_T;
Type *INT32PTR_T;
Type *FLOAT_T;
Type *FLOATPTR_T;

/*
 * use CMinusfBuilder::Scope to construct scopes
 * scope.enter: enter a new scope
 * scope.exit: exit current scope
 * scope.push: add a new binding to current scope
 * scope.find: find and return the value bound to the name
 */

Value* CminusfBuilder::visit(ASTProgram &node) {
    VOID_T = module->get_void_type();
    INT1_T = module->get_int1_type();
    INT32_T = module->get_int32_type();
    INT32PTR_T = module->get_int32_ptr_type();
    FLOAT_T = module->get_float_type();
    FLOATPTR_T = module->get_float_ptr_type();

    Value *ret_val = nullptr;
    for (auto &decl : node.declarations) {
        ret_val = decl->accept(*this);
    }
    return ret_val;
}

Value* CminusfBuilder::visit(ASTNum &node) {
    if(node.type == TYPE_INT)
        return CONST_INT(node.i_val);
    else if(node.type == TYPE_FLOAT)
        return CONST_FP(node.f_val);
    else
        LOG(ERROR) << "Unknown type.";
    return nullptr;
}

Value* CminusfBuilder::visit(ASTVarDeclaration &node) {
    Type *var_type = nullptr;
    if (node.type == TYPE_INT)
        var_type = INT32_T;
    else if (node.type == TYPE_FLOAT)
        var_type = FLOAT_T;
    else if (node.type == TYPE_VOID)
        LOG(ERROR) << "It makes no sense to declare a void variable.";
    else 
        LOG(ERROR) << "Unknown type.";
    if (node.num == nullptr) {
        // code.num will be nullptr if the variable is not an array.
        if (scope.in_global()){
            auto initializer = ConstantZero::get(var_type, module.get());
            scope.push(node.id, GlobalVariable::create(node.id, module.get(), var_type, false, initializer));
        }
        else {
            scope.push(node.id, builder->create_alloca(var_type));
        }
    }
    else {
        auto const_val = static_cast<ConstantInt *>(node.num->accept(*this));
        auto array_type = ArrayType::get(var_type, const_val->get_value());
        if(scope.in_global()){
            auto initializer = ConstantZero::get(var_type, module.get());
            scope.push(node.id, GlobalVariable::create(node.id, module.get(), array_type, false, initializer));
        }
        else{
            auto ptr = builder->create_alloca(array_type);
            scope.push(node.id, ptr);
        }
    }
   
    return nullptr;
}

Value* CminusfBuilder::visit(ASTFunDeclaration &node) {
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
        if(param->type == TYPE_INT){
            if (param->isarray)
                param_types.push_back(INT32PTR_T);
            else param_types.push_back(INT32_T);
        }
        else if(param->type == TYPE_FLOAT){
            if (param->isarray)
                param_types.push_back(FLOATPTR_T);    
            else param_types.push_back(FLOAT_T);
        }
        else
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
            // LOG(DEBUG)<<scope.find("a")->get_type()->get_pointer_element_type()->print();
    }
    node.compound_stmt->accept(*this);
    if (not builder->get_insert_block()->is_terminated())
    {
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

Value* CminusfBuilder::visit(ASTParam &node) {
    // TODO: This function is empty now.
    // Add some code here.
    return nullptr;
}

Value* CminusfBuilder::visit(ASTCompoundStmt &node) {
    // to deal with complex statements.
    
    scope.enter();
    // FIXME: There will be a bug if the function has a local variable named the same as a function parameter.
    // but it seems the code doesn't care about the repeated name.
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

Value* CminusfBuilder::visit(ASTExpressionStmt &node) {
    node.expression->accept(*this);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTSelectionStmt &node) {
    auto expression_ret = node.expression->accept(*this);
    if(expression_ret->get_type()->is_float_type())
        expression_ret = builder->create_fcmp_ne(expression_ret, CONST_FP(0.));
    else if(expression_ret->get_type()->is_int32_type())
        expression_ret = builder->create_icmp_ne(expression_ret, CONST_INT(0));
    if(node.else_statement != nullptr){
        auto trueBB = BasicBlock::create(module.get(), "", context.func);
        auto falseBB = BasicBlock::create(module.get(), "", context.func);
        auto mergeBB = BasicBlock::create(module.get(), "", context.func);
        builder->create_cond_br(expression_ret, trueBB, falseBB);

        builder->set_insert_point(trueBB);
        node.if_statement->accept(*this);
        builder->create_br(mergeBB);

        builder->set_insert_point(falseBB);
        node.else_statement->accept(*this);
        builder->create_br(mergeBB);

        builder->set_insert_point(mergeBB);
    }
    else{
        auto trueBB = BasicBlock::create(module.get(), "", context.func);
        auto mergeBB = BasicBlock::create(module.get(), "", context.func);

        builder->create_cond_br(expression_ret, trueBB, mergeBB);

        builder->set_insert_point(trueBB);
        node.if_statement->accept(*this);
        builder->create_br(mergeBB);

        builder->set_insert_point(mergeBB);
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTIterationStmt &node) {
    auto condBB = BasicBlock::create(module.get(), "", context.func);
    auto bodyBB = BasicBlock::create(module.get(), "", context.func);
    auto mergeBB = BasicBlock::create(module.get(), "", context.func);

    builder->create_br(condBB);

    builder->set_insert_point(condBB);
    auto expression_ret = node.expression->accept(*this);
    if(expression_ret->get_type()->is_float_type())
        expression_ret = builder->create_fcmp_ne(expression_ret, CONST_FP(0.));
    else if(expression_ret->get_type()->is_int32_type())
        expression_ret = builder->create_icmp_ne(expression_ret, CONST_INT(0));
    builder->create_cond_br(expression_ret, bodyBB, mergeBB);

    builder->set_insert_point(bodyBB);
    node.statement->accept(*this);
    builder->create_br(condBB);

    builder->set_insert_point(mergeBB);
    return nullptr;
}

Value* CminusfBuilder::visit(ASTReturnStmt &node) {
    if (node.expression == nullptr) {
        builder->create_void_ret();
        return nullptr;
    } else {
        // You need to solve other return cases (e.g. return an integer).
        auto expression_ret = node.expression->accept(*this);
        builder->create_ret(expression_ret);
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTVar &node) {
    if(node.expression == nullptr){
        auto ptr = scope.find(node.id);
        // LOG(DEBUG)<<ptr->get_type()->is_array_type();
        // LOG(DEBUG)<<ptr->get_type()->get_pointer_element_type()->is_array_type();
        if(context.isLValue)return ptr;
        else if(ptr->get_type()->get_pointer_element_type()->is_array_type())
            return builder->create_gep(ptr, {CONST_INT(0),CONST_INT(0)});
        else return builder->create_load(ptr);
    }
    else{
        auto expression_ret = node.expression->accept(*this);
        if(not expression_ret->get_type()->is_integer_type())
            expression_ret = builder->create_fptosi(expression_ret, INT32_T);

        auto exceptBB = BasicBlock::create(module.get(), "", context.func);
        auto mergeBB = BasicBlock::create(module.get(), "", context.func);

        auto icmp = builder->create_icmp_lt(expression_ret, CONST_INT(0));
        builder->create_cond_br(icmp, exceptBB, mergeBB);

        builder->set_insert_point(exceptBB);
        builder->create_call(scope.find("neg_idx_except"), {});
        builder->create_br(mergeBB);

        builder->set_insert_point(mergeBB);
        auto var = scope.find(node.id);
        GetElementPtrInst *ptr;
        if(var->get_type()->get_pointer_element_type()->is_array_type())
            ptr = builder->create_gep(var, {CONST_INT(0), expression_ret});
        else 
            ptr = builder->create_gep(var, {expression_ret});
        if(context.isLValue)return ptr;
        else return builder->create_load(ptr);
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTAssignExpression &node) {
    context.isLValue = true;
    auto var = node.var->accept(*this);
    context.isLValue = false;
    auto var_type = var->get_type()->get_pointer_element_type();
    auto expression_ret = node.expression->accept(*this);
    if(var_type->is_float_type()){
        if(not expression_ret->get_type()->is_float_type()){
            expression_ret = builder->create_sitofp(expression_ret, FLOAT_T);
        }
    }
    else if(var_type->is_integer_type()){
        if(expression_ret->get_type()->is_float_type())
            expression_ret = builder->create_fptosi(expression_ret, INT32_T);
        else if(expression_ret->get_type()->is_int1_type())
            expression_ret = builder->create_zext(expression_ret, INT32_T);
    }
    builder->create_store(expression_ret, var);
    return expression_ret;
}

Value* CminusfBuilder::visit(ASTSimpleExpression &node) {
    if(node.additive_expression_r == nullptr){
        return node.additive_expression_l->accept(*this);
    }
    else {
        auto lhs = node.additive_expression_l->accept(*this);
        auto rhs = node.additive_expression_r->accept(*this);
        if(lhs->get_type()->is_float_type() or rhs->get_type()->is_float_type()){
            if(not lhs->get_type()->is_float_type())
                lhs = builder->create_sitofp(lhs, FLOAT_T);
            if(not rhs->get_type()->is_float_type())
                rhs = builder->create_sitofp(rhs, FLOAT_T);
            if(node.op == OP_LT){
                return builder->create_fcmp_lt(lhs, rhs);
            }
            else if(node.op == OP_LE){
                return builder->create_fcmp_le(lhs, rhs);
            }
            else if(node.op == OP_GT){
                return builder->create_fcmp_gt(lhs, rhs);
            }
            else if(node.op == OP_GE){
                return builder->create_fcmp_ge(lhs, rhs);
            }
            else if(node.op == OP_EQ){
                return builder->create_fcmp_eq(lhs, rhs);
            }
            else if(node.op == OP_NEQ){
                return builder->create_fcmp_ne(lhs, rhs);
            }
        }
        else {
            if(lhs->get_type()->is_int1_type())
                lhs = builder->create_zext(lhs, INT32_T);
            if(rhs->get_type()->is_int1_type())
                rhs = builder->create_zext(rhs, INT32_T);
            if(node.op == OP_LT){
                return builder->create_icmp_lt(lhs, rhs);
            }
            else if(node.op == OP_LE){
                return builder->create_icmp_le(lhs, rhs);
            }
            else if(node.op == OP_GT){
                return builder->create_icmp_gt(lhs, rhs);
            }
            else if(node.op == OP_GE){
                return builder->create_icmp_ge(lhs, rhs);
            }
            else if(node.op == OP_EQ){
                return builder->create_icmp_eq(lhs, rhs);
            }
            else if(node.op == OP_NEQ){
                return builder->create_icmp_ne(lhs, rhs);
            }
        }
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTAdditiveExpression &node) {
    if(node.additive_expression == nullptr){
        return node.term->accept(*this);
    }
    else{
        auto lhs = node.additive_expression->accept(*this);
        auto rhs = node.term->accept(*this);
        if(lhs->get_type()->is_float_type() or rhs->get_type()->is_float_type()){
            if(not lhs->get_type()->is_float_type())
                lhs = builder->create_sitofp(lhs, FLOAT_T);
            if(not rhs->get_type()->is_float_type())
                rhs = builder->create_sitofp(rhs, FLOAT_T);
            if(node.op == OP_PLUS){
                return builder->create_fadd(lhs, rhs);
            }
            else if(node.op == OP_MINUS){
                return builder->create_fsub(lhs, rhs);
            }
        }
        else{
            if(lhs->get_type()->is_int1_type())
                lhs = builder->create_zext(lhs, INT32_T);
            if(rhs->get_type()->is_int1_type())
                rhs = builder->create_zext(rhs, INT32_T);
            if(node.op == OP_PLUS){
                return builder->create_iadd(lhs, rhs);
            }
            else if(node.op == OP_MINUS){
                return builder->create_isub(lhs, rhs);
            }
        }
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTTerm &node) {
    if(node.term == nullptr){
        return node.factor->accept(*this);
    }
    else{
        auto lhs = node.term->accept(*this);
        auto rhs = node.factor->accept(*this);
        if(lhs->get_type()->is_float_type() or rhs->get_type()->is_float_type()){
            if(not lhs->get_type()->is_float_type())
                lhs = builder->create_sitofp(lhs, FLOAT_T);
            if(not rhs->get_type()->is_float_type())
                rhs = builder->create_sitofp(rhs, FLOAT_T);
            if(node.op == OP_MUL){
                return builder->create_fmul(lhs, rhs);
            }
            else if(node.op == OP_DIV){
                return builder->create_fdiv(lhs, rhs);
            }
        }
        else{
            if(lhs->get_type()->is_int1_type())
                lhs = builder->create_zext(lhs, INT32_T);
            if(rhs->get_type()->is_int1_type())
                rhs = builder->create_zext(rhs, INT32_T);
            if(node.op == OP_MUL){
                return builder->create_imul(lhs, rhs);
            }
            else if(node.op == OP_DIV){
                return builder->create_isdiv(lhs, rhs);
            }
        }
    }
    return nullptr;
}

Value* CminusfBuilder::visit(ASTCall &node) {
    LOG(DEBUG)<<node.id;
    std::vector<Value *> args;
    for (auto &arg : node.args) {
        args.push_back(arg->accept(*this));
        // LOG(DEBUG)<<args.back()->get_type()->get_pointer_element_type()->print();
    }
    auto func = scope.find(node.id);
    auto func_type = static_cast<FunctionType *>(func->get_type());
    if(func_type->get_num_of_args() != args.size()){
        LOG(DEBUG) << func_type->get_num_of_args();
        LOG(DEBUG) << args.size();
        LOG(ERROR) << "The number of arguments is not equal to the number of parameters.";
    }
    for (unsigned int i = 0 ; i < args.size(); i++){
        if(func_type->get_param_type(i)->is_float_type() and not args[i]->get_type()->is_float_type())
            args[i] = builder->create_sitofp(args[i], FLOAT_T);
        else if(func_type->get_param_type(i)->is_integer_type()){
            if(args[i]->get_type()->is_int1_type())
                args[i] = builder->create_zext(args[i], INT32_T);
            else if(args[i]->get_type()->is_float_type())
                args[i] = builder->create_fptosi(args[i], INT32_T);
        }
        LOG(DEBUG)<<func_type->get_param_type(i)->print();
        LOG(DEBUG)<<args[i]->get_type()->print();
    }
    return builder->create_call(func, args);
}
