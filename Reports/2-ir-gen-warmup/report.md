# lab2 实验报告
PB19111733 陈鑫

## 问题1: cpp 与 .ll 的对应

请说明你的 cpp 代码片段和 .ll 的每个 BasicBlock 的对应关系。

* assign.c

  ```
      ;define a[10], a[0]=10
      %1 = alloca [10 x i32], align 4
      %2=getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 0
      store i32 10, i32* %2, align 4
      
      ;calculate a[0]*2
      %3=load i32, i32* %2, align 4
      %4 = alloca i32, align 4
      store i32 2, i32* %4
      %5=load i32, i32* %4
      %6=mul i32 %3, %5
      
      %7=getelementptr inbounds [10 x i32], [10 x i32]* %1, i64 0, i64 1
      store i32 %6, i32* %7, align 4
      %8=load i32, i32* %7, align 4
      ret i32 %8
  ```

  ```cpp
      // main函数
      auto mainFun = Function::create(FunctionType::get(Int32Type, {}), "main", module);
      auto bb=BasicBlock::create(module, "entry", mainFun);
      builder->set_insert_point(bb);
  
      //定义a[10]
      auto Int32PtrType=Type::get_int32_ptr_type(module);
      auto apAlloca=builder->create_alloca(Int32PtrType);
      auto *arrayType=ArrayType::get(Int32Type,10);
      auto a=builder->create_alloca(arrayType);
  
      //a[0]=10
      auto a0GEP=builder->create_gep(a, {CONST_INT(0),CONST_INT(0)});
      builder->create_store(CONST_INT(10),a0GEP);
  
      //a[1]
      auto a1GEP=builder->create_gep(a,{CONST_INT(0),CONST_INT(1)});
      
      //a[0]*2
      auto a0load=builder->create_load(a0GEP);
      auto mul=builder->create_imul(a0load,CONST_INT(2));
      builder->create_store(mul,a1GEP);
      auto a1load=builder->create_load(a1GEP);
      builder->create_ret(a1load);
  ```

* fun.c

  函数callee部分

  ```cpp
      //callee函数
      std::vector<Type *>Ints(1,Int32Type);
      auto calleefy=FunctionType::get(Int32Type,Ints);
      auto Callee=Function::create(calleefy,"callee",module);
      auto bb=BasicBlock::create(module,"entry",Callee);
      builder->set_insert_point(bb);
  
      auto retAlloca=builder->create_alloca(Int32Type);
      auto aAlloca=builder->create_alloca(Int32Type);
      
      std::vector<Value *> args;
      for(auto arg=Callee->arg_begin();arg!=Callee->arg_end();arg++)
      {
          args.push_back(*arg);
      }
  
      builder->create_store(args[0],aAlloca);
      auto aload=builder->create_load(aAlloca);
      auto mul=builder->create_imul(aload,CONST_INT(2));
      builder->create_store(mul,retAlloca);
      auto retload=builder->create_load(retAlloca);
      builder->create_ret(retload);
  ```

  

  ```
  define dso_local i32 @callee(i32 %0) #0 {
      %2=alloca i32, align 4
      store i32 2, i32* %2, align 4
      %3=load i32, i32* %2, align 4
  
      ;calculate a*2
      %4=mul i32 %3, %0
      ret i32 %4
  }
  ```

  主函数部分

  ```cpp
      //main
      auto mainFun = Function::create(FunctionType::get(Int32Type, {}),
                                      "main", module);
      bb = BasicBlock::create(module, "entry", mainFun);
      builder->set_insert_point(bb);
  
      retAlloca=builder->create_alloca(Int32Type);
      builder->create_store(CONST_INT(0),retAlloca);
  	
      auto call=builder->create_call(Callee,{CONST_INT(110)});
      builder->create_ret(call);
  ```

  对应.ll中

  ```
  define dso_local i32 @main() #1 {
      %1=alloca i32, align 4
      store i32 110, i32* %1, align 4
      %2=load i32, i32* %1, align 4
  
      ;call function callee
      %3=call i32 @callee(i32 %2)
      ret i32 %3
  }
  ```

* if.c

  初始定义赋值部分：

  ```cpp
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
  ```

  ```
      ;define 5.555
      %1=alloca double, align 4
      store double 5.555, double* %1, align 4
      %2=load double, double* %1, align 4
      
      %3=alloca i32, align 4
      store i32 1, i32* %3, align 4
      %4=load i32, i32* %3, align 4
      %5=sitofp i32 %4 to double
  
      %6=fcmp ugt double %2, %5
  
      %7=alloca i32, align 4
      br i1 %6, label %iftrue, label %iffalse
  ```

  判断的true部分：

  ```cpp
  	//trueBB部分
  	DEBUG_OUTPUT
          builder->set_insert_point(trueBB);
      builder->create_store(CONST_INT(233),retAlloca);
      builder->create_br(retBB);
  ```

  ```
  iftrue:
      store i32 233, i32* %7, align 4
      br label %end
  ```

  判断的false部分：

  ```cpp
      //falseBB部分
      builder->set_insert_point(falseBB);
      builder->create_store(CONST_INT(0),retAlloca);
      builder->create_br(retBB);
  ```

  ```
  iffalse:
      store i32 0, i32* %7, align 4
      br label %end
  ```

  结束：

  ```cpp
      //结束
      builder->set_insert_point(retBB);
      auto retload=builder->create_load(retAlloca);
      builder->create_ret(retload);
  ```

  ```
  end:
      %8=load i32, i32* %7, align 4
      ret i32 %8
  ```

* while.c

  初始定义部分：

  ```cpp
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
  ```

  ```
      ;define a=10, i=0
      ;store a in %2    store i in %1
      %1=alloca i32, align 4
      %2=alloca i32, align 4
  
      store i32 0, i32* %1, align 4
      %3=load i32, i32* %1, align 4
      store i32 10, i32* %2, align 4
      %4=load i32, i32* %2, align 4
  
      %5=icmp slt i32 %3, 10
      br i1 1, label %while, label %end
  ```

  while主体以及判断：

  ```cpp
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
  ```

  ```
  while:
      ;calculate and store a and i
      %6=load i32, i32* %1, align 4
      %7=add i32 %6, 1
      store i32 %7, i32* %1, align 4
      %8=load i32, i32* %2, align 4
      %9=add i32 %8, %7
      store i32 %9, i32* %2, align 4
      
      %10=icmp slt i32 %7, 10
  
      br i1 %10, label %while, label %end
  ```

  结束：

  ```cpp
      builder->set_insert_point(whileend);
      aload=builder->create_load(aAlloca);
      builder->create_ret(aload);
  ```

  ```
  end:
      %11=load i32, i32* %2, align 4
      ret i32 %11

## 问题2: Visitor Pattern

分析 `calc` 程序在输入为 `4 * (8 + 4 - 1) / 2` 时的行为：
1. 请画出该表达式对应的抽象语法树（使用 `calc_ast.hpp` 中的 `CalcAST*` 类型和在该类型中存储的值来表示），并给节点使用数字编号。
2. 请指出示例代码在用访问者模式遍历该语法树时的遍历顺序。

序列请按如下格式指明（序号为问题 2.1 中的编号）：  
3->2->5->1



1. 语法树:

   ```log
   .
   root													1
   └── expression											2
   	└── term											3
   		├── term										4
   		│	├── term									5
   		│	│	└── factor								6
   		│	│		└── val								7
   		│	│			└── 4							8
   		│	├── OP_MUL									9
   		│	│	└── *									10
   		│	└── factor									11
   		│		├── (									12
   		│		├── expression							13
   		│		│	├── expression						14
   		│		│	│	├── expression					15
   		│		│	│	│	└── term					16
   		│		│	│	│		└── factor				17
   		│		│	│	│			└── val				18
   		│		│	│	│				└── 8			19
   		│		│	│	├── OP_PLUS						20
   		│		│	│	│	└── +						21
   		│		│	│	└── term						22
   		│		│	│		└── factor					23
   		│		│	│			└── val					24
   		│		│	│				└── 4				25
   		│		│	├── OP_MINUS						26
   		│		│	│	└── -							27
   		│		│	└── term							28
   		│		│		└── factor						29
   		│		│			└── val						30
   		│		│				└── 1					31
   		│		└── )									32
   		├── OP_DIV										33
   		│	└── /										34
   		└── factor										35
   			└── val										36
   				└── 2									37
   ```
   
2. 遍历顺序：

   1->2->3->4->5->6->7->8->11->13->14->15->16->17->18->19->22->23->24->25->20->21->28->29->30->31->26->27->35->36->37->33->34

## 问题3: getelementptr

请给出 `IR.md` 中提到的两种 getelementptr 用法的区别,并稍加解释:
  - `%2 = getelementptr [10 x i32], [10 x i32]* %1, i32 0, i32 %0`

  - `%2 = getelementptr i32, i32* %1 i32 %0`

    getelementptr可以用来获得指向数组的元素和指向结构体成员的指针

    1. 第一个用法是获得一个大小10的整型数组的第%0个元素，即num[0], %1指的是这个数组，后面的第一个0是偏移量，即前进0x[10x i32]；
    2. 第二个用法中%1是一个i32类型的指针，偏移量是%0 x i32

## 实验难点

描述在实验中遇到的问题、分析和解决方案。

* 编写.ll文件中，对数组定义和赋值不够熟悉，在查阅了getelementptr的用法后，利用其实现数组的定义
* 在对if_hand.ll中的5.555分配空间并赋值时，总是报错，显示“floating point constant invalid for type”，最终将float换成double解决问题，但是错误原因还是不太理解明白
* 编写.cpp文件时，对样例的结构理解起来比较困难，最终根据注释和报错信息完成.cpp文件

## 实验反馈

吐槽?建议?

​	实验总体来说难度适中，代码样例简单全面，但是还是对部分模块的理解有困难。比如.ll的数组定义，分支跳转，.cpp的模块跳转等。
