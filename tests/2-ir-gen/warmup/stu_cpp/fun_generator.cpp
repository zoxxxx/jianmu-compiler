#include "BasicBlock.hpp"
#include "Constant.hpp"
#include "Function.hpp"
#include "IRBuilder.hpp"
#include "Module.hpp"
#include "Type.hpp"

#include <iostream>
#include <memory>
#include <new>

// 定义一个从常数值获取/创建 ConstantInt 类实例化的宏，方便多次调用
#define CONST_INT(num) \
    ConstantInt::get(num, module)

// 定义一个从常数值获取/创建 ConstantFP 类实例化的宏，方便多次调用
#define CONST_FP(num) \
    ConstantFP::get(num, module)
int main() {
    auto module = new Module();
    auto builder = new IRBuilder(nullptr, module);
    auto Int32Type = module->get_int32_type();
    auto calleeFunTy = FunctionType::get(Int32Type, {Int32Type});
    auto calleeFun = Function::create(calleeFunTy, "callee", module);
    auto bb = BasicBlock::create(module, "entry", calleeFun);
    builder->set_insert_point(bb);
    std::vector<Value *> args;
    for (auto &arg: calleeFun->get_args()) {
        args.push_back(&arg);
    }
    auto retValue = builder->create_imul(args[0], CONST_INT(2));
    builder->create_ret(retValue);

    auto mainFunTy = FunctionType::get(Int32Type, {});
    auto mainFun = Function::create(mainFunTy, "main", module);
    bb = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(bb);
    auto callRet = builder->create_call(calleeFun, {CONST_INT(110)});
    builder->create_ret(callRet);

    std::cout << module->print();
    delete module;

    return 0;
}
