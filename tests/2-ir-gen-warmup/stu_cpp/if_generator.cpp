#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRBuilder.h"
#include "Module.h"
#include "Type.h"

#include <iostream>
#include <memory>

#ifdef DEBUG                                             // 用于调试信息,大家可以在编译过程中通过" -DDEBUG"来开启这一选项
#define DEBUG_OUTPUT std::cout << __LINE__ << std::endl; // 输出行号的简单示例
#else
#define DEBUG_OUTPUT
#endif

#define CONST_INT(num) \
  ConstantInt::get(num, module)

#define CONST_FP(num) \
  ConstantFP::get(num, module) // 得到常数值的表示,方便后面多次用到

int main(){
    auto module = new Module("Cminus code"); // module name是什么无关紧要
    auto builder = new IRBuilder(nullptr, module);
    Type *Int32Type = Type::get_int32_type(module);
    Type *FloatType = Type::get_float_type(module);

    auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                  "main", module);
    auto bb = BasicBlock::create(module, "entry", mainFun);
    // BasicBlock的名字在生成中无所谓,但是可以方便阅读
    builder->set_insert_point(bb);

    auto retAlloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(0), retAlloca); // 默认 ret 0

    //a赋值
    auto aAlloca=builder->create_alloca(FloatType);
    builder->create_store(CONST_FP(5.555),aAlloca);
    auto aload=builder->create_load(aAlloca);

    auto fcmp=builder->create_fcmp_gt(aload,CONST_FP(1));
    auto trueBB=BasicBlock::create(module,"trueBB",mainFun);
    auto falseBB=BasicBlock::create(module,"falseBB",mainFun);
    auto retBB=BasicBlock::create(module,"",mainFun);
    //判断
    auto br=builder->create_cond_br(fcmp,trueBB,falseBB);

    //tureBB部分
    DEBUG_OUTPUT
        builder->set_insert_point(trueBB);
    builder->create_store(CONST_INT(233),retAlloca);
    builder->create_br(retBB);

    //falseBB部分
    builder->set_insert_point(falseBB);
    builder->create_store(CONST_INT(0),retAlloca);
    builder->create_br(retBB);

    //结束
    builder->set_insert_point(retBB);
    auto retload=builder->create_load(retAlloca);
    builder->create_ret(retload);
    std::cout << module->print();
    delete module;
    return 0;

}