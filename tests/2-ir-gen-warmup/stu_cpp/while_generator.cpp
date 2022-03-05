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

    auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                  "main", module);
    auto bb = BasicBlock::create(module, "entry", mainFun);
    // BasicBlock的名字在生成中无所谓,但是可以方便阅读
    builder->set_insert_point(bb);

    auto retAlloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(0), retAlloca); // 默认 ret 0

    auto aAlloca=builder->create_alloca(Int32Type);
    auto iAlloca=builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(10),aAlloca);
    builder->create_store(CONST_INT(0),iAlloca);
    auto aload=builder->create_load(aAlloca);
    auto iload=builder->create_load(iAlloca);

    auto whilebody=BasicBlock::create(module,"whilebody",mainFun);
    auto whileend=BasicBlock::create(module,"whileend",mainFun);

    auto icmp=builder->create_icmp_lt(iload,CONST_INT(10));
    auto br=builder->create_cond_br(icmp,whilebody,whileend);

    DEBUG_OUTPUT
        builder->set_insert_point(whilebody);
    iload=builder->create_load(iAlloca);
    auto i_new_load=builder->create_iadd(CONST_INT(1),iload);
    builder->create_store(i_new_load,iAlloca);
    iload=builder->create_load(iAlloca);
    aload=builder->create_load(aAlloca);
    auto a_new_load=builder->create_iadd(iload,aload);
    builder->create_store(a_new_load,aAlloca);
    icmp=builder->create_icmp_lt(iload,CONST_INT(10));
    br=builder->create_cond_br(icmp,whilebody,whileend);
    builder->create_br(whileend);

    builder->set_insert_point(whileend);
    aload=builder->create_load(aAlloca);
    builder->create_ret(aload);
    std::cout << module->print();
    delete module;
    return 0;

}