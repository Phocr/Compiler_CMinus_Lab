source_filename="if.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

define dso_local i32 @main() #0 {
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

iftrue:
    store i32 233, i32* %7, align 4
    br label %end

iffalse:
    store i32 0, i32* %7, align 4
    br label %end

end:
    %8=load i32, i32* %7, align 4
    ret i32 %8
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="all" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0-4ubuntu1 "}