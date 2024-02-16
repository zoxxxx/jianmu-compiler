#include "cminusf_builder.hpp"
#include "Constant.hpp"
#include "Function.hpp"
#include "GlobalVariable.hpp"
#include "Instruction.hpp"
#include "Type.hpp"
#include "ast.hpp"
#include "logging.hpp"
#include <algorithm>

struct ConstStruct{
    int type; // 1 int 2 float 3 arrayint 4 arrayflaot
    vector <int> dim; // if array, dimesion is in here
    vector <int> DataI; // store confirmed data in array
    vector <float> DataF;
    union{
        int ConstInt;
        float ConstFp;
    }SingleData;
};

class myScope {
  public:
    // enter a new scope
    void enter() { inner.emplace_back(); }

    // exit a scope
    void exit() { inner.pop_back(); }

    bool in_global() { return inner.size() == 1; }

    // push a name to scope
    // return true if successful
    // return false if this name already exits
    bool push(const std::string& name, ConstStruct* val) {
        auto result = inner[inner.size() - 1].insert({name, val});
        return result.second;
    }

    ConstStruct* find(const std::string& name) {
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
    std::vector<std::map<std::string, ConstStruct*> > inner;
}myscope;

ConstStruct* CminusfBuilder::visit(ASTProgram &node) {
    (node.Vec[0])->accept(*this);
    return null;
}

ConstStruct* CminusfBuilder::visit(ASTStmt &node){
    return null;
}
ConstStruct* CminusfBuilder::visit(ASTFuncDef &node){
    return null;
}
ConstStruct* CminusfBuilder::visit(ASTConstDecl &node){
    for(auto &nxt:node.const_defs){
        nxt->accept(*this);
    }
    return null;
}
ConstStruct* CminusfBuilder::visit(ASTVarDecl &node){
    return null;
}
ConstStruct* CminusfBuilder::visit(ASTConstDef &node){
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
ConstStruct* CminusfBuilder::visit(ASTConstInitVal &node){
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

ConstStruct* CminusfBuilder::visit(ASTVarDefList &node){
    return null;
}
ConstStruct* CminusfBuilder::visit(ASTVarDef &node){
    return null;
}
ConstStruct* CminusfBuilder::visit(ASTInitVal &node){
    return null;
}
ConstStruct* CminusfBuilder::visit(ASTBlock &node){
    scope.enter();
    for(auto &nxt : decls_and_stmts){
        nxt->accept(*this);
    }
    scope.exit();
}
ConstStruct* CminusfBuilder::visit(ASTFuncFParam &node){
    return null;
}

ConstStruct* CminusfBuilder::visit(ASTLVal &node){
    //todo
}
ConstStruct* CminusfBuilder::visit(ASTCond &node){
    return node.lorexp->accept(*this);
}
    
    
ConstStruct* CminusfBuilder::visit(ASTAddExp &node){
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
ConstStruct* CminusfBuilder::visit(ASTLOrExp &node){
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

ConstStruct* CminusfBuilder::visit(ASTIntConst &node){
    ConstStruct* NumRec = new ConstStruct;
    NumRec->type = 1;
    NumRec->SingleData.ConstInt = node.num;
    return NumRec;
}
ConstStruct* CminusfBuilder::visit(ASTFloatConst &node){
    ConstStruct* NumRec = new ConstStruct;
    NumRec.type = 2;
    NumRec->SingleData.ConstFp = node.num;
    return NumRec;
}
ConstStruct* CminusfBuilder::visit(ASTUnaryExp &node){
    //todo
}

ConstStruct* CminusfBuilder::visit(ASTBinaryExp &node){
    auto r1 = node.opcode1 -> accept(*this);
    auto r2 = node.opcode2 -> accept(*this);
    ConstStruct* rr = new ConstStruct;
    if(node.is_bool_exp()){
        
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
        }
    }
    delete r1;
    delete r2;
    return rr;
}
ConstStruct* CminusfBuilder::visit(ASTRelExp &node){
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
        if(node.op == OP_LE){
            rr->SingleData.ConstInt = r1->SingleData.ConstFp <= r2->SingleData.ConstFp;
        }else if(node.op == OP_GE){
            rr->SingleData.ConstInt = r1->SingleData.ConstFp >= r2->SingleData.ConstFp;
        }else if(node.op == OP_LT){
            rr->SingleData.ConstInt = r1->SingleData.ConstFp < r2->SingleData.ConstFp;
        }else if(node.op == OP_GT){
            rr->SingleData.ConstInt = r1->SingleData.ConstFp > r2->SingleData.ConstFp;
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
        }
    }
    delete r1;
    delete r2;
    return rr;
}
ConstStruct* CminusfBuilder::visit(ASTEqExp &node){
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
        if(node.op == OP_EQ){
            rr->SingleData.ConstInt = (r1->SingleData.ConstFp == r2->SingleData.ConstFp);
        }else if(node.op == OP_NE){
            rr->SingleData.ConstInt = (r1->SingleData.ConstFp != r2->SingleData.ConstFp);
        }
    }else{
        if(node.op == OP_EQ){
            rr->SingleData.ConstInt = (r1->SingleData.ConstInt == r2->SingleData.ConstInt);
        }else if(node.op == OP_NE){
            rr->SingleData.ConstInt = (r1->SingleData.ConstInt != r2->SingleData.ConstInt);
        }
    }
    delete r1;
    delete r2;
    return rr;
}
ConstStruct* CminusfBuilder::visit(ASTLAndExp &node){
    auto r1 = node.lhs -> accept(*this);
    auto r2 = node.rhs -> accept(*this);
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
        rr->SingleData.ConstInt = (r1->SingleData.ConstFp && r2->SingleData.ConstFp);
    }else{
        rr->SingleData.ConstInt = (r1->SingleData.ConstInt && r2->SingleData.ConstInt);
    }
    delete r1;
    delete r2;
    return rr;
}
