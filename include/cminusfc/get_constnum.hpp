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
};

class GetConst : public ASTVisitor {
public:
private:
    virtual ConstStruct *visit(ASTProgram &) override final;
    virtual ConstStruct *visit(ASTConstDecl &) override final;
    virtual ConstStruct *visit(ASTConstDef &) override final;
    virtual ConstStruct *visit(ASTConstInitVal &) override final;
    virtual ConstStruct *visit(ASTVarDecl &) override final;
    virtual ConstStruct *visit(ASTVarDef &) override final;
    virtual ConstStruct *visit(ASTInitVal &) override final;
    virtual ConstStruct *visit(ASTFuncDef &) override final;
    virtual ConstStruct *visit(ASTFParam &) override final;
    virtual ConstStruct *visit(ASTBlock &) override final;
    virtual ConstStruct *visit(ASTExpStmt &) override final;
    virtual ConstStruct *visit(ASTAssignStmt &) override final;
    virtual ConstStruct *visit(ASTBlockStmt &) override final;
    virtual ConstStruct *visit(ASTSelectionStmt &) override final;
    virtual ConstStruct *visit(ASTIterationStmt &) override final;
    virtual ConstStruct *visit(ASTReturnStmt &) override final;
    virtual ConstStruct *visit(ASTBreakStmt &) override final;
    virtual ConstStruct *visit(ASTContinueStmt &) override final;
    virtual ConstStruct *visit(ASTLVal &) override final;
    virtual ConstStruct *visit(ASTNumber &) override final;
    virtual ConstStruct *visit(ASTUnaryExp &) override final;
    virtual ConstStruct *visit(ASTBinaryExp &) override final;
    virtual ConstStruct *visit(ASTConstExp &) override final;
    virtual ConstStruct *visit(ASTCond &) override final;
    myScope myscope;
};
