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
    auto module = new Module;
    auto builder = new IRBuilder(nullptr, module);
    auto Int32Type = module->get_int32_type();
    auto mainFun = Function::create(FunctionType::get(Int32Type, {}), "main", module);
    auto bb = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(bb);
    auto aAlloca = builder->create_alloca(Int32Type);
    auto iAlloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(10), aAlloca);
    builder->create_store(CONST_INT(0), iAlloca);
    auto cmpBB = BasicBlock::create(module, "cmpBB", mainFun);
    auto whileBB = BasicBlock::create(module, "whileBB", mainFun);
    auto retBB = BasicBlock::create(module, "retBB", mainFun);
    builder->create_br(cmpBB);

    builder->set_insert_point(cmpBB);
    auto ival = builder->create_load(iAlloca);
    auto cmp = builder->create_icmp_lt(ival, CONST_INT(10));
    builder->create_cond_br(cmp, whileBB, retBB);

    builder->set_insert_point(whileBB);
    auto newival = builder->create_iadd(ival, CONST_INT(1));
    builder->create_store(newival, iAlloca);
    auto aval = builder->create_load(aAlloca);
    auto newaval = builder->create_iadd(aval, newival);
    builder->create_store(newaval, aAlloca);
    builder->create_br(cmpBB);

    builder->set_insert_point(retBB);
    auto retval = builder->create_load(aAlloca);
    builder->create_ret(retval);

    std::cout<<module->print();
    delete module;
    return 0;
}
