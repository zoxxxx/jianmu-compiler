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
    auto Int32Type = module->get_int32_type();
    auto mainfun  = new Function(FunctionType::get(Int32Type, {}), "main", module);
    auto bb = BasicBlock::create(module, "entry", mainfun);
    auto builder = new IRBuilder(nullptr, module);
    builder->set_insert_point(bb);
    auto arrayType = ArrayType::get(Int32Type, 10);
    auto a = builder->create_alloca(arrayType);
    auto a0 = builder->create_gep(a, {CONST_INT(0), CONST_INT(0)});
    builder->create_store(CONST_INT(10), a0);
    auto a1 = builder->create_gep(a, {CONST_INT(0), CONST_INT(0)});
    auto val = builder->create_load(a1);
    auto val2 = builder->create_imul(val, CONST_INT(2));
    auto a2 = builder->create_gep(a, {CONST_INT(0), CONST_INT(1)});
    builder->create_store(val2, a2);
    auto a3 = builder->create_gep(a, {CONST_INT(0), CONST_INT(1)});
    auto ans = builder->create_load(a3);
    builder->create_ret(ans);
    std::cout<<module->print();
    delete module;
    return 0;
    
}
