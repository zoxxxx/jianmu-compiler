#include "cminusf_builder.hpp"
#include "Constant.hpp"
#include "Function.hpp"
#include "GlobalVariable.hpp"
#include "Instruction.hpp"
#include "Type.hpp"
#include "ast.hpp"
#include "logging.hpp"
#include "get_constnum.hpp"
#include <algorithm>

ConstStruct* GetConst::visit(ASTProgram &node) {
    (node.Vec[0])->accept(*this);
    return null;
}

ConstStruct* GetConst::visit(ASTStmt &node){
    return null;
}
ConstStruct* GetConst::visit(ASTFuncDef &node){
    return null;
}
ConstStruct* GetConst::visit(ASTConstDecl &node){
    for(auto &nxt:node.const_defs){
        nxt->accept(*this);
    }
    return null;
}
ConstStruct* GetConst::visit(ASTVarDecl &node){
    return null;
}
ConstStruct* GetConst::visit(ASTConstDef &node){
    ConstStruct* ConstMessage = new ConstStruct;
    if(node.is_array){
        for(auto &nxt : node.array_size){
            auto size = nxt->accept(*this);
            ConstMessage->dim.push_back(size->SingleData.ConstInt); //size
        }
        int MAXN = 1;
        for(auto x : ConstMessage.dim) MAXN *= x; //求出数组大小
        if(node.type == TYPE_INT){
            ConstMessage -> type = 3;
            ConstMessage.DataI.resize(MAXN,0);
            context.Record = ConstMessage; 
            context.nowpos.resize(dim.size(),0); //当前的位置，全部初始化为0
            context.frontpos = -1; //不看比0低的位
            for(auto &nxt : ConstMessage.init_vals){
                nxt -> accept(*this);
            }
        }else {
            ConstMessage -> type = 4;
            ConstMessage.DataF.resize(MAXN,0);
            context.Record = ConstMessage; //记录一下下面改
            context.nowpos.resize(dim.size(),0); //当前的位置，全部初始化为0
            context.frontpos = -1; //不看比0低的位
            for(auto &nxt : ConstMessage.init_vals){
                nxt -> accept(*this);
            }
        }
    }else{
        if(node.type == TYPE_INT){
            ConstMessage -> type = 1;
            auto DownData = (node.init_val)->accept(*this);
            ConstMessage->SingleData.ConstInt=DownData->SingleData.ConstInt;
        }else {
            ConstMessage -> type = 2;
            auto DownData = (node.init_val)->accept(*this);
            ConstMessage->SingleData.ConstFp=DownData->SingleData.ConstFp;
        }
    }
    myscope.push(node.id,ConstMessage);
    //这里需要添加把ConstMessage加入AST该结点中
    return null;
}
ConstStruct* GetConst::visit(ASTConstInitVal &node){
    auto lstpos = context.nowpos;
    int pos = context.frontpos+1;
    int lstpos = context.frontpos; //因为递归，所以需要记录一下
    for(int i=pos;i<context.nowpos.size();i++) if(context.nowpos[i]) pos = i;
    if(is_empty){
        context.nowpos = lstpos;
        context.nowpos[pos] ++;
    }else if(init_vals.size() == 0){
        //把数填进去 context.Record
        int finalpos = 0;
        for(int i=0;i<context.nowpos.size();i++){
            finalpos = finalpos * context.Record->dim[i] + cotext.nowpos[i];
        }
        auto RC = context.Record;
        auto ppt = node.const_exp -> accept(*this);
        context.Record = RC;
        if(context.Record -> Type == 3){
            context.Record -> DataI[finalpos] = ppt->SingleData.ConstInt;
        }else {
            if(ppt->SingleData.type == 1){
                context.Record -> DataF[finalpos] = ppt->SingleData.ConstInt;
            }else{
                context.Record -> DataF[finalpos] = ppt->SingleData.ConstFp;
            }
        }
        context.nowpos = lstpos;
        context.nowpos[context.nowpos.size()]++;
    }else{
        for(auto &nxt : node.init_vals)
            nxt -> accept(*this);
        contextnowpos = lstpos;
        context.nowpos[pos]++;
    }
    for(int i=context.nowpos.size();i>=1;i--){
        if(context.nowpos[i] >= context.Record->dim[i]){
            context.nowpos[i-1] += context.nowpos[i]/context.Record->dim[i];
            context.nowpos[i] -= context.nowpos[i]%context.Record->dim[i];
        }
    }
    context.frontpos = lstpos;
}

ConstStruct* GetConst::visit(ASTVarDefList &node){
    return null;
}
ConstStruct* GetConst::visit(ASTVarDef &node){
    return null;
}
ConstStruct* GetConst::visit(ASTInitVal &node){
    return null;
}
ConstStruct* GetConst::visit(ASTBlock &node){
    scope.enter();
    for(auto &nxt : decls_and_stmts){
        nxt->accept(*this);
    }
    scope.exit();
}
ConstStruct* GetConst::visit(ASTFuncFParam &node){
    return null;
}

ConstStruct* GetConst::visit(ASTLVal &node){
    ConstStruct* now = myscope.find(node.id);
    int pos = 0;
    int ct = 0;
    for(auto &nxt : array_exp){
        auto *dt = nxt -> accept(*this);
        pos = pos * now->dim[ct] + dt->SingleData->ConstInt;
        ct++;
    }
    ConstStruct* res = new ConstStruct;
    res->type = now->type - 2;
    if(res->type == 1) res->SingleData->ConstInt = now->DataI[pos];
    else res->SingleData->ConstFp = now->DataF[pos];
    return res;
}
ConstStruct* GetConst::visit(ASTCond &node){
    return node.lorexp->accept(*this);
}
    
    
ConstStruct* GetConst::visit(ASTAddExp &node){
    auto r1 = node.opcode1 -> accept(*this);
    auto r2 = node.opcode2 -> accept(*this);
    ConstStruct* rr = new ConstStruct;
    if(r1->type == 2 || r2->type == 2){
        rr->type = 2;
        if(r1->type == 1) {
            r1->type = 2;
            r1->SingleData.ConstFp = r1->SingleData.ConstInt;
        }
        if(r2->type == 1) {
            r2->type = 2;
            r2->SingleData.ConstFp = r2->SingleData.ConstInt;
        }
        if(node.op == OP_ADD){
            rr->SingleData.ConstFp = r1->SingleData.ConstFp + r2->SingleData.ConstFp;
        }else if(node.op == OP_SUB){
            rr->SingleData.ConstFp = r1->SingleData.ConstFp - r2->SingleData.ConstFp;
        }
    }else{
        rr->type = 1;
        if(node.op == OP_ADD){
            rr->SingleData.ConstInt = r1->SingleData.ConstInt + r2->SingleData.ConstInt;
        }else if(node.op == OP_SUB){
            rr->SingleData.ConstInt = r1->SingleData.ConstInt - r2->SingleData.ConstInt;
        }
    }
    delete r1;
    delete r2;
    return rr;
}
ConstStruct* GetConst::visit(ASTLOrExp &node){
    auto r1 = node.opcode1 -> accept(*this);
    auto r2 = node.opcode2 -> accept(*this);
    ConstStruct* rr = new ConstStruct;
    rr->type = 1;
    if(r1->type == 2 || r2->type == 2){
        if(r1->type == 1) {
            r1->type = 2;
            r1->SingleData.ConstFp = r1->SingleData.ConstInt;
        }
        if(r2->type == 1) {
            r2->type = 2;
            r2->SingleData.ConstFp = r2->SingleData.ConstInt;
        }
        rr->SingleData.ConstInt = (r1->SingleData.ConstFp || r2->SingleData.ConstFp);
    }else{
        rr->SingleData.ConstInt = (r1->SingleData.ConstInt || r2->SingleData.ConstInt);
    }
    delete r1;
    delete r2;
    return rr;
}

ConstStruct* GetConst::visit(ASTNumber &node){
    if(node.type == TYPE_INT){
        ConstStruct* NumRec = new ConstStruct;
        NumRec->type = 1;
        NumRec->SingleData.ConstInt = node.value;
        return NumRec;
    }else{
        ConstStruct* NumRec = new ConstStruct;
        NumRec->type = 2;
        NumRec->SingleData.ConstFp = node.value;
        return NumRec;
    }
}

ConstStruct* GetConst::visit(ASTUnaryExp &node){
    if(node.has_unary_op == true){
        ConstStruct* NumRec = node.exp -> accept(*this);y
        if(node.op == OP_POS){
            return NumRec;
        }else if(node.op == OP_NEG){
            if(NumRec->op == 1){
                NumRec->SingleData.ConstInt *= -1;
            }else{
                NumRec->SingleData.ConstFp *= -1;
            }
            return NumRec;
        }else{
            if(NumRec->op == 1){
                NumRec->SingleData.ConstInt = !NumRec.SingleData.ConstInt;
            }else{
                NumRec->SingleData.ConstFp = !NumRec.SingleData.ConstFp;
            }
            return NumRec;
        }
    }else{
        if(node.is_func_call()){
            return null;
        }else{
            if(node.is_number()){
                return node.number ->accept(*this);
            }else{
                return node.l_val ->accept(*this);
            }
        }
    }
}

ConstStruct* GetConst::visit(ASTBinaryExp &node){
    auto r1 = node.opcode1 -> accept(*this);
    auto r2 = node.opcode2 -> accept(*this);
    ConstStruct* rr = new ConstStruct;
    if(node.is_bool_exp()){
        rr->type = 1;
        if(r1->type == 2 || r2->type == 2){
            if(r1->type == 1) {
                r1->type = 2;
                r1->SingleData.ConstFp = r1->SingleData.ConstInt;
            }
            if(r2->type == 1) {
                r2->type = 2;
                r2->SingleData.ConstFp = r2->SingleData.ConstInt;
            }
            if(node.op == OP_LE){
                rr->SingleData.ConstInt = r1->SingleData.ConstFp <= r2->SingleData.ConstFp;
            }else if(node.op == OP_GE){
                rr->SingleData.ConstInt = r1->SingleData.ConstFp >= r2->SingleData.ConstFp;
            }else if(node.op == OP_LT){
                rr->SingleData.ConstInt = r1->SingleData.ConstFp < r2->SingleData.ConstFp;
            }else if(node.op == OP_GT){
                rr->SingleData.ConstInt = r1->SingleData.ConstFp > r2->SingleData.ConstFp;
            }else if(node.op == OP_EQ){
                rr->SingleData.ConstInt = r1->SingleData.ConstFp == r2->SingleData.ConstFp;
            }else if(node.op == OP_NEQ){
                rr->SingleData.ConstInt = r1->SingleData.ConstFp != r2->SingleData.ConstFp;
            }else if(node.op == OP_AND){
                rr->SingleData.ConstInt = r1->SingleData.ConstFp && r2->SingleData.ConstFp;
            }else if(node.op == OP_OR){
                rr->SingleData.ConstInt = r1->SingleData.ConstFp || r2->SingleData.ConstFp;
            }
        
        }else{
            if(node.op == OP_LE){
                rr->SingleData.ConstInt = r1->SingleData.ConstInt <= r2->SingleData.ConstInt;
            }else if(node.op == OP_GE){
                rr->SingleData.ConstInt = r1->SingleData.ConstInt >= r2->SingleData.ConstInt;
            }else if(node.op == OP_LT){
                rr->SingleData.ConstInt = r1->SingleData.ConstInt < r2->SingleData.ConstInt;
            }else if(node.op == OP_GT){
                rr->SingleData.ConstInt = r1->SingleData.ConstInt > r2->SingleData.ConstInt;
            }else if(node.op == OP_EQ){
                rr->SingleData.ConstInt = r1->SingleData.ConstInt == r2->SingleData.ConstInt;
            }else if(node.op == OP_NEQ){
                rr->SingleData.ConstInt = r1->SingleData.ConstInt != r2->SingleData.ConstInt;
            }else if(node.op == OP_AND){
                rr->SingleData.ConstInt = r1->SingleData.ConstInt && r2->SingleData.ConstInt;
            }else if(node.op == OP_OR){
                rr->SingleData.ConstInt = r1->SingleData.ConstInt || r2->SingleData.ConstInt;
            }
        }
    }else{
        if(r1->type == 2 || r2->type == 2){
            rr->type = 2;
            if(r1->type == 1) {
                r1->type = 2;
                r1->SingleData.ConstFp = r1->SingleData.ConstInt;
            }
            if(r2->type == 1) {
                r2->type = 2;
                r2->SingleData.ConstFp = r2->SingleData.ConstInt;
            }
            if(node.op == OP_MUL){
                rr->SingleData.ConstFp = r1->SingleData.ConstFp * r2->SingleData.ConstFp;
            }else if(node.op == OP_DIV){
                rr->SingleData.ConstFp = r1->SingleData.ConstFp / r2->SingleData.ConstFp;
            }else if(node.op == OP_MOD){
                rr->SingleData.ConstFp = r1->SingleData.ConstFp % r2->SingleData.ConstFp;
            }else if(node.op == OP_PLUS){
                rr->SingleData.ConstFp = r1->SingleData.ConstFp + r2->SingleData.ConstFp;
            }else if(node.op == OP_MINUS){
                rr->SingleData.ConstFp = r1->SingleData.ConstFp - r2->SingleData.ConstFp;
            }
        }else{
            rr->type = 1;
            if(node.op == OP_MUL){
                rr->SingleData.ConstInt = r1->SingleData.ConstInt * r2->SingleData.ConstInt;
            }else if(node.op == OP_DIV){
                rr->SingleData.ConstInt = r1->SingleData.ConstInt / r2->SingleData.ConstInt;
            }else if(node.op == OP_MOD){
                rr->SingleData.ConstInt = r1->SingleData.ConstInt % r2->SingleData.ConstInt;
            }else if(node.op == OP_PLUS){
                rr->SingleData.ConstInt = r1->SingleData.ConstInt + r2->SingleData.ConstInt;
            }else if(node.op == OP_MINUS){
                rr->SingleData.ConstInt = r1->SingleData.ConstInt - r2->SingleData.ConstInt;
            }
        }
    }
    delete r1;
    delete r2;
    return rr;
}
