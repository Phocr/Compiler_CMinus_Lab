source_filename="while.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

define dso_local i32 @main() #0 {
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

end:
    %11=load i32, i32* %2, align 4
    ret i32 %11
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}