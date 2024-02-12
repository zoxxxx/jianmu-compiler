#include "ast.hpp"
#include "Value.hpp"
#include "logging.hpp"
#include "syntax_tree.h"

#include <cstring>
#include <iostream>
#include <memory>
#include <stack>

#define _AST_NODE_ERROR_                                                       \
    std::cerr << "Abort due to node cast error."                               \
                 "Contact with TAs to solve your problem."                     \
              << std::endl;                                                    \
    std::abort();
#define _STR_EQ(a, b) (strcmp((a), (b)) == 0)

void AST::run_visitor(ASTVisitor &visitor) { root->accept(visitor); }

AST::AST(syntax_tree *s) {
    if (s == nullptr) {
        std::cerr << "empty input tree!" << std::endl;
        std::abort();
    }
    auto node = transform_node_iter(s->root);
    del_syntax_tree(s);
    root = std::static_pointer_cast<ASTProgram>(node);
}

std::shared_ptr<ASTNode> AST::transform_node_iter(syntax_tree_node *n) {
    if (_STR_EQ(n->name, "Program")) {
        // auto node = new ASTProgram();
        auto node = std::make_shared<ASTProgram>();
        // flatten declaration list
        std::stack<syntax_tree_node *> s;
        auto list_ptr = n->children[0];
        while (list_ptr->children_num == 2) {
            s.push(list_ptr->children[1]);
            list_ptr = list_ptr->children[0];
        }
        s.push(list_ptr->children[0]);

        while (!s.empty()) {
            auto child_node = transform_node_iter(s.top());
            if(child_node->is_const_decl()){
                node->defs_and_decls.push_back(
                    std::static_pointer_cast<ASTConstDecl>(child_node));
            }
            else if(child_node->is_var_decl()){
                node->defs_and_decls.push_back(
                    std::static_pointer_cast<ASTVarDecl>(child_node));
            }
            else if(child_node->is_func_def()){
                node->defs_and_decls.push_back(
                    std::static_pointer_cast<ASTFuncDef>(child_node));
            }
            else{
                std::cerr << "Unknown type of declaration" << std::endl;
                std::abort();
            }
            s.pop();
        }
        return node;
    }
    else if (_STR_EQ(n->name, "Decl")) {
        return transform_node_iter(n->children[0]);
    }
    else if(_STR_EQ(n->name, "ConstDecl")) {
        auto node = std::make_shared<ASTConstDecl>();
        SysyType type;
        if (_STR_EQ(n->children[1]->children[0]->name, "int")) {
            type = TYPE_INT;
        }
        else {
           type = TYPE_FLOAT;
        }
        
        auto list_ptr = n->children[2];
        std::stack<syntax_tree_node *> s;
        while(list_ptr->children_num == 2) {
            s.push(list_ptr->children[1]);
            list_ptr = list_ptr->children[0];
        }
        s.push(list_ptr->children[0]);
        while(!s.empty()) {
            auto child_node =
                std::static_pointer_cast<ASTConstDef>(transform_node_iter(s.top()));
            child_node->type = type;
            node->const_defs.push_back(child_node);
            s.pop();
        }
        return node;
    }
    else if (_STR_EQ(n->name, "ConstDef")) {
        auto node = std::make_shared<ASTConstDef>();
        node->id = n->children[0]->name;
        if(n->children[2]->children_num == 0) {
            node->is_array = false;
        }
        else {
            node->is_array = true;
            auto list_ptr = n->children[2];
            std::stack<syntax_tree_node *> s;
            while(list_ptr->children_num == 4) {
                s.push(list_ptr->children[3]);
                list_ptr = list_ptr->children[0];
            }
            while(!s.empty()) {
                auto child_node =
                    std::static_pointer_cast<ASTConstExp>(transform_node_iter(s.top()));
                node->array_size.push_back(child_node);
                s.pop();
            }
        }
        node->init_val =
            std::static_pointer_cast<ASTConstInitVal>(transform_node_iter(n->children[4]));
        return node;
    }
    else if(_STR_EQ(n->name, "ConstInitVal")) {
        auto node = std::make_shared<ASTConstInitVal>();
        if(n->children_num == 1) {
            node->is_single_exp = true;
            node->const_exp =
                std::static_pointer_cast<ASTConstExp>(transform_node_iter(n->children[0]));
        }
        else {
            node->is_single_exp = false;
            auto list_ptr = n->children[1];
            std::stack<syntax_tree_node *> s;
            while(list_ptr->children_num == 3) {
                s.push(list_ptr->children[2]);
                list_ptr = list_ptr->children[0];
            }
            if(list_ptr->children_num == 1){
                s.push(list_ptr->children[0]);
            }
            while(!s.empty()) {
                auto child_node =
                    std::static_pointer_cast<ASTConstInitVal>(transform_node_iter(s.top()));
                node->init_vals.push_back(child_node);
                s.pop();
            }
            return node;
        }
    }
    else if(_STR_EQ(n->name, "VarDecl")) {
        auto node = std::make_shared<ASTVarDecl>();
        SysyType type;
        if (_STR_EQ(n->children[0]->children[0]->name, "int")) {
            type = TYPE_INT;
        }
        else {
           type = TYPE_FLOAT;
        }
        auto list_ptr = n->children[1];
        std::stack<syntax_tree_node *> s;
        while(list_ptr->children_num == 2) {
            s.push(list_ptr->children[1]);
            list_ptr = list_ptr->children[0];
        }
        s.push(list_ptr->children[0]);
        while(!s.empty()){
            auto child_node =
                std::static_pointer_cast<ASTVarDef>(transform_node_iter(s.top()));
            child_node->type = type;
            node->var_defs.push_back(child_node);
            s.pop();
        }
        return node;
    }
    else if (_STR_EQ(n->name, "VarDef")) {
        auto node = std::make_shared<ASTVarDef>();
        node->id = n->children[0]->name;
        if(n->children[2]->children_num == 0) {
            node->is_array = false;
        }
        else {
            node->is_array = true;
            auto list_ptr = n->children[2];
            std::stack<syntax_tree_node *> s;
            while(list_ptr->children_num == 4) {
                s.push(list_ptr->children[3]);
                list_ptr = list_ptr->children[0];
            }
            while(!s.empty()) {
                auto child_node =
                    std::static_pointer_cast<ASTConstExp>(transform_node_iter(s.top()));
                node->array_size.push_back(child_node);
                s.pop();
            }
        }
        return node;
    }
    else if(_STR_EQ(n->name, "FuncDef")) {
        auto node = std::make_shared<ASTFuncDef>();
        if(_STR_EQ(n->children[0]->children[0]->name, "void")) {
            node->type = TYPE_VOID;
        }
        else if(_STR_EQ(n->children[0]->children[0]->name, "int")) {
            node->type = TYPE_INT;
        }
        else {
            node->type = TYPE_FLOAT;
        }
        node->id = n->children[1]->name;
        if (n->children_num == 6) {
            auto list_ptr = n->children[3];
            std::stack<syntax_tree_node *> s;
            while (list_ptr->children_num == 3) {
                s.push(list_ptr->children[2]);
                list_ptr = list_ptr->children[0];
            }
            s.push(list_ptr->children[0]);
            while (!s.empty()) {
                auto child_node =
                    std::static_pointer_cast<ASTFParam>(transform_node_iter(s.top()));
                node->params.push_back(child_node);
                s.pop();
            }
            node->block =
                std::static_pointer_cast<ASTBlock>(transform_node_iter(n->children[5]));
        }
        else {
            node->block =
                std::static_pointer_cast<ASTBlock>(transform_node_iter(n->children[3]));
        }
        return node;
    }
    else if (_STR_EQ(n->name, "FuncFParam")) {
        auto node = std::make_shared<ASTFParam>();
        if (_STR_EQ(n->children[0]->children[0]->name, "int")) {
            node->type = TYPE_INT;
        }
        else {
            node->type = TYPE_FLOAT;
        }
        node->id = n->children[1]->name;
        if (n->children_num > 2){
            node->is_array = true;
            auto list_ptr = n->children[4];
            std::stack<syntax_tree_node *> s;
            while (list_ptr->children_num > 0) {
                s.push(list_ptr->children[2]);
                list_ptr = list_ptr->children[0];
            }
            while (!s.empty()) {
                auto child_node =
                    std::static_pointer_cast<ASTConstExp>(transform_node_iter(s.top()));
                node->array_size.push_back(child_node);
                s.pop();
            }
        }

        return node;
    }
    else if (_STR_EQ(n->name, "Block")) {
        auto node = std::make_shared<ASTBlock>();
        auto list_ptr = n->children[1];
        std::stack<syntax_tree_node *> s;
        while (list_ptr->children_num > 0) {
            s.push(list_ptr->children[1]);
            list_ptr = list_ptr->children[0];
        }
        while(!s.empty()) {
            auto child_node = transform_node_iter(s.top());
            if (child_node->is_var_decl()) {
                node->decls_and_stmts.push_back(
                    std::static_pointer_cast<ASTVarDecl>(child_node));
            }
            else if (child_node->is_const_decl()) {
                node->decls_and_stmts.push_back(
                    std::static_pointer_cast<ASTConstDecl>(child_node));
            }
            else if (child_node->is_stmt()) {
                node->decls_and_stmts.push_back(
                    std::static_pointer_cast<ASTStmt>(child_node));
            }
            s.pop();
        }
        return node;
    }
    else if (_STR_EQ(n->name, "BlockItem")) {
        return transform_node_iter(n->children[0]);
    }
    else if (_STR_EQ(n->name, "Stmt")) {
        if(_STR_EQ(n->children[1]->name, "=")) {
            auto node = std::make_shared<ASTAssignStmt>();
            node->l_val = std::static_pointer_cast<ASTLVal>(transform_node_iter(n->children[0]));
            node->exp = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[2]));
        }
        else if(_STR_EQ(n->children[0]->name, "Exp")) {
            auto node = std::make_shared<ASTExpStmt>();
            node->exp = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[0]));
        }
        else if(_STR_EQ(n->children[0]->name, "Block")) {
            auto node = std::make_shared<ASTBlockStmt>();
            node->block = std::static_pointer_cast<ASTBlock>(transform_node_iter(n->children[0]));
        }
        else if(_STR_EQ(n->children[0]->name, "if")) {
            auto node = std::make_shared<ASTSelectionStmt>();
            node->cond = std::static_pointer_cast<ASTCond>(transform_node_iter(n->children[2]));
            node->if_stmt = std::static_pointer_cast<ASTStmt>(transform_node_iter(n->children[4]));
            if(n->children_num == 7) {
                node->has_else = true;
                node->else_stmt = std::static_pointer_cast<ASTStmt>(transform_node_iter(n->children[6]));
            }
            else {
                node->has_else = false;
            }
        }
        else if(_STR_EQ(n->children[0]->name, "while")) {
            auto node = std::make_shared<ASTIterationStmt>();
            node->cond = std::static_pointer_cast<ASTCond>(transform_node_iter(n->children[2]));
            node->stmt = std::static_pointer_cast<ASTStmt>(transform_node_iter(n->children[4]));
        }
        else if(_STR_EQ(n->children[0]->name, "return")) {
            auto node = std::make_shared<ASTReturnStmt>();
            if(n->children_num == 3) {
                node->is_empty = false;
                node->exp = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[1]));
            }
            else {
                node->is_empty = true;
            }
        }
        else if(_STR_EQ(n->children[0]->name, "break")) {
            auto node = std::make_shared<ASTBreakStmt>();
        }
        else if(_STR_EQ(n->children[0]->name, "continue")) {
            auto node = std::make_shared<ASTContinueStmt>();
        }
    }
    else if (_STR_EQ(n->name, "Exp")) {
        return transform_node_iter(n->children[0]);
    }
    else if (_STR_EQ(n->name, "Cond")) {
        return transform_node_iter(n->children[0]);
    }
    else if (_STR_EQ(n->name, "LVal")) {
        auto node = std::make_shared<ASTLVal>();
        node->id = n->children[0]->name;
        auto list_ptr = n->children[1];
        std::stack<syntax_tree_node *> s;
        while(list_ptr->children_num == 2) {
            s.push(list_ptr->children[1]);
            list_ptr = list_ptr->children[0];
        }
        s.push(list_ptr->children[0]);
        while(!s.empty()) {
            auto child_node = transform_node_iter(s.top());
            node->array_exp.push_back(std::static_pointer_cast<ASTExp>(child_node));
        }
        return node;
    }
    else if (_STR_EQ(n->name, "Number")) {
        auto node = std::make_shared<ASTNumber>();
        if(_STR_EQ(n->children[0]->name, "INT")) {
            node->type = TYPE_INT;
            node->value = std::stoi(n->children[0]->name, nullptr, 0);
        }
        else {
            node->type = TYPE_FLOAT;
            node->value = std::stof(n->children[0]->name, nullptr);
        }
        return node;
    }
    else if (_STR_EQ(n->name, "UnaryExp")) {
        auto node = std::make_shared<ASTUnaryExp>();
        if(n->children_num == 1) {
            auto syntax_child = n->children[0]->children[0];
            if(_STR_EQ(syntax_child->name, "Number")) {
                node->number = std::static_pointer_cast<ASTNumber>(transform_node_iter(syntax_child));
            }
            else if(_STR_EQ(syntax_child->name, "LVal")) {
                node->l_val = std::static_pointer_cast<ASTLVal>(transform_node_iter(syntax_child));
            }
            else if(_STR_EQ(syntax_child->name, "Exp")) {
                node->exp = std::static_pointer_cast<ASTExp>(transform_node_iter(syntax_child));
            }
        }
        else if(n->children_num == 2) {
            node->has_unary_op = true;
            if(_STR_EQ(n->children[0]->name, "+")) {
                node->op = OP_POS;
            }
            else if(_STR_EQ(n->children[0]->name, "-")) {
                node->op = OP_NEG;
            }
            else if(_STR_EQ(n->children[0]->name, "!")) {
                node->op = OP_NOT;
            }
            node->exp = std::static_pointer_cast<ASTUnaryExp>(transform_node_iter(n->children[1]));
        }
        else if(n->children_num == 4 || n->children_num == 3) {
            node->func_call_id = n->children[0]->name;
            if(n->children_num == 4) {
                auto list_ptr = n->children[2];
                std::stack<syntax_tree_node *> s;
                while(list_ptr->children_num == 3) {
                    s.push(list_ptr->children[2]);
                    list_ptr = list_ptr->children[0];
                }
                s.push(list_ptr->children[0]);
                while(!s.empty()) {
                    auto child_node = transform_node_iter(s.top());
                    node->func_call_args.push_back(std::static_pointer_cast<ASTExp>(child_node));
                    s.pop();
                }
            }
        }
    }
    else if (_STR_EQ(n->name, "MulExp")) {
        if(n->children[0]->children_num == 1) {
            return transform_node_iter(n->children[0]);
        }
        else {
            auto node = std::make_shared<ASTBinaryExp>();
            node->lhs = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[0]));
            node->rhs = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[2]));
            if(_STR_EQ(n->children[1]->name, "*")) {
                node->op = OP_MUL;
            }
            else if (_STR_EQ(n->children[1]->name, "/")){
                node->op = OP_DIV;
            }
            else if (_STR_EQ(n->children[1]->name, "%")){
                node->op = OP_MOD;
            }
            return node;
        }
    }
    else if (_STR_EQ(n->name, "AddExp")) {
        if(n->children[0]->children_num == 1) {
            return transform_node_iter(n->children[0]);
        }
        else {
            auto node = std::make_shared<ASTBinaryExp>();
            node->lhs = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[0]));
            node->rhs = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[2]));
            if(_STR_EQ(n->children[1]->name, "+")) {
                node->op = OP_PLUS;
            }
            else if(_STR_EQ(n->children[1]->name, "-")) {
                node->op = OP_MINUS;
            }
            return node;
        }
    }
    else if (_STR_EQ(n->name, "RelExp")) {
        if(n->children[0]->children_num == 1) {
            return transform_node_iter(n->children[0]);
        }
        else {
            auto node = std::make_shared<ASTBinaryExp>();
            node->lhs = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[0]));
            node->rhs = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[2]));
            if(_STR_EQ(n->children[1]->name, "<")) {
                node->op = OP_LT;
            }
            else if(_STR_EQ(n->children[1]->name, "<=")) {
                node->op = OP_LE;
            }
            else if(_STR_EQ(n->children[1]->name, ">")) {
                node->op = OP_GT;
            }
            else if(_STR_EQ(n->children[1]->name, ">=")){
                node->op = OP_GE;
            }
            return node;
        }
    }
    else if (_STR_EQ(n->name, "EqExp")) {
        if(n->children[0]->children_num == 1) {
            return transform_node_iter(n->children[0]);
        }
        else {
            auto node = std::make_shared<ASTBinaryExp>();
            node->lhs = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[0]));
            node->rhs = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[2]));
            if(_STR_EQ(n->children[1]->name, "==")) {
                node->op = OP_EQ;
            }
            else if(_STR_EQ(n->children[1]->name, "!=")) {
                node->op = OP_NEQ;
            }
            return node;
        }
    }
    else if (_STR_EQ(n->name, "LAndExp")) {
        if(n->children[0]->children_num == 1) {
            return transform_node_iter(n->children[0]);
        }
        else {
            auto node = std::make_shared<ASTBinaryExp>();
            node->lhs = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[0]));
            node->rhs = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[2]));
            node->op = OP_AND;
            return node;
        }
    }
    else if (_STR_EQ(n->name, "LOrExp")) {
        if(n->children[0]->children_num == 1) {
            return transform_node_iter(n->children[0]);
        }
        else {
            auto node = std::make_shared<ASTBinaryExp>();
            node->lhs = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[0]));
            node->rhs = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[2]));
            node->op = OP_OR;
            return node;
        }
    }
    else if (_STR_EQ(n->name, "ConstExp")) {
        auto node = std::make_shared<ASTConstExp>();
        node->exp = std::static_pointer_cast<ASTExp>(transform_node_iter(n->children[0]));
        return node;
    }
    else {
        std::cerr << "Unknown node type: " << n->name << std::endl;
        std::abort();
    }
    return nullptr;
}

Value* ASTProgram::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTNum::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTVarDeclaration::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTFunDeclaration::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTParam::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTCompoundStmt::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTExpressionStmt::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTSelectionStmt::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTIterationStmt::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTReturnStmt::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTAssignExpression::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTSimpleExpression::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTAdditiveExpression::accept(ASTVisitor &visitor) {
    return visitor.visit(*this);
}
Value* ASTVar::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTTerm::accept(ASTVisitor &visitor) { return visitor.visit(*this); }
Value* ASTCall::accept(ASTVisitor &visitor) { return visitor.visit(*this); }

#define _DEBUG_PRINT_N_(N)                                                     \
    { std::cout << std::string(N, '-'); }

Value* ASTPrinter::visit(ASTProgram &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "program" << std::endl;
    add_depth();
    for (auto decl : node.declarations) {
        decl->accept(*this);
    }
    remove_depth();
    return nullptr;
}

Value* ASTPrinter::visit(ASTNum &node) {
    _DEBUG_PRINT_N_(depth);
    if (node.type == TYPE_INT) {
        std::cout << "num (int): " << node.i_val << std::endl;
    } else if (node.type == TYPE_FLOAT) {
        std::cout << "num (float): " << node.f_val << std::endl;
    } else {
        _AST_NODE_ERROR_
    }
    return nullptr;
}

Value* ASTPrinter::visit(ASTVarDeclaration &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "var-declaration: " << node.id;
    if (node.num != nullptr) {
        std::cout << "[]" << std::endl;
        add_depth();
        node.num->accept(*this);
        remove_depth();
        return nullptr;
    }
    std::cout << std::endl;
    return nullptr;
}

Value* ASTPrinter::visit(ASTFunDeclaration &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "fun-declaration: " << node.id << std::endl;
    add_depth();
    for (auto param : node.params) {
        param->accept(*this);
    }

    node.compound_stmt->accept(*this);
    remove_depth();
    return nullptr;
}

Value* ASTPrinter::visit(ASTParam &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "param: " << node.id;
    if (node.isarray)
        std::cout << "[]";
    std::cout << std::endl;
    return nullptr;
}

Value* ASTPrinter::visit(ASTCompoundStmt &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "compound-stmt" << std::endl;
    add_depth();
    for (auto decl : node.local_declarations) {
        decl->accept(*this);
    }

    for (auto stmt : node.statement_list) {
        stmt->accept(*this);
    }
    remove_depth();
    return nullptr;
}

Value* ASTPrinter::visit(ASTExpressionStmt &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "expression-stmt" << std::endl;
    add_depth();
    if (node.expression != nullptr)
        node.expression->accept(*this);
    remove_depth();
    return nullptr;
}

Value* ASTPrinter::visit(ASTSelectionStmt &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "selection-stmt" << std::endl;
    add_depth();
    node.expression->accept(*this);
    node.if_statement->accept(*this);
    if (node.else_statement != nullptr)
        node.else_statement->accept(*this);
    remove_depth();
    return nullptr;
}

Value* ASTPrinter::visit(ASTIterationStmt &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "iteration-stmt" << std::endl;
    add_depth();
    node.expression->accept(*this);
    node.statement->accept(*this);
    remove_depth();
    return nullptr;
}

Value* ASTPrinter::visit(ASTReturnStmt &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "return-stmt";
    if (node.expression == nullptr) {
        std::cout << ": void" << std::endl;
    } else {
        std::cout << std::endl;
        add_depth();
        node.expression->accept(*this);
        remove_depth();
    }
    return nullptr;
}

Value* ASTPrinter::visit(ASTAssignExpression &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "assign-expression" << std::endl;
    add_depth();
    node.var->accept(*this);
    node.expression->accept(*this);
    remove_depth();
    return nullptr;
}

Value* ASTPrinter::visit(ASTSimpleExpression &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "simple-expression";
    if (node.additive_expression_r == nullptr) {
        std::cout << std::endl;
    } else {
        std::cout << ": ";
        if (node.op == OP_LT) {
            std::cout << "<";
        } else if (node.op == OP_LE) {
            std::cout << "<=";
        } else if (node.op == OP_GE) {
            std::cout << ">=";
        } else if (node.op == OP_GT) {
            std::cout << ">";
        } else if (node.op == OP_EQ) {
            std::cout << "==";
        } else if (node.op == OP_NEQ) {
            std::cout << "!=";
        } else {
            std::abort();
        }
        std::cout << std::endl;
    }
    add_depth();
    node.additive_expression_l->accept(*this);
    if (node.additive_expression_r != nullptr)
        node.additive_expression_r->accept(*this);
    remove_depth();
    return nullptr;
}

Value* ASTPrinter::visit(ASTAdditiveExpression &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "additive-expression";
    if (node.additive_expression == nullptr) {
        std::cout << std::endl;
    } else {
        std::cout << ": ";
        if (node.op == OP_PLUS) {
            std::cout << "+";
        } else if (node.op == OP_MINUS) {
            std::cout << "-";
        } else {
            std::abort();
        }
        std::cout << std::endl;
    }
    add_depth();
    if (node.additive_expression != nullptr)
        node.additive_expression->accept(*this);
    node.term->accept(*this);
    remove_depth();
    return nullptr;
}

Value* ASTPrinter::visit(ASTVar &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "var: " << node.id;
    if (node.expression != nullptr) {
        std::cout << "[]" << std::endl;
        add_depth();
        node.expression->accept(*this);
        remove_depth();
        return nullptr;
    }
    std::cout << std::endl;
    return nullptr;
}

Value* ASTPrinter::visit(ASTTerm &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "term";
    if (node.term == nullptr) {
        std::cout << std::endl;
    } else {
        std::cout << ": ";
        if (node.op == OP_MUL) {
            std::cout << "*";
        } else if (node.op == OP_DIV) {
            std::cout << "/";
        } else {
            std::abort();
        }
        std::cout << std::endl;
    }
    add_depth();
    if (node.term != nullptr)
        node.term->accept(*this);

    node.factor->accept(*this);
    remove_depth();
    return nullptr;
}

Value* ASTPrinter::visit(ASTCall &node) {
    _DEBUG_PRINT_N_(depth);
    std::cout << "call: " << node.id << "()" << std::endl;
    add_depth();
    for (auto arg : node.args) {
        arg->accept(*this);
    }
    remove_depth();
    return nullptr;
}
