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
    auto FloatType = module->get_float_type();
    auto Int32Type = module->get_int32_type();
    auto mainFun = Function::create(FunctionType::get(Int32Type, {}), "main", module);
    auto bb = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(bb);
    auto aAlloca = builder->create_alloca(FloatType);
    builder->create_store(CONST_FP(5.555), aAlloca); 
    auto trueBB = BasicBlock::create(module, "trueBB", mainFun);
    auto falseBB = BasicBlock::create(module, "falseBB", mainFun);
    auto cmp = builder->create_fcmp_gt(builder->create_load(aAlloca), CONST_FP(1));
    builder->create_cond_br(cmp, trueBB, falseBB);
    builder->set_insert_point(trueBB);
    builder->create_ret(CONST_INT(233));
    builder->set_insert_point(falseBB);
    builder->create_ret(CONST_INT(0));

    std::cout<<module->print();
    delete module;
    return 0;
}
