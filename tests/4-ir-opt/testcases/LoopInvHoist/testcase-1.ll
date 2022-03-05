; ModuleID = 'cminus'
source_filename = "testcase-1.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label_entry:
  %op0 = alloca i32
  %op1 = alloca i32
  %op2 = alloca i32
  store i32 1, i32* %op0
  br label %label3
label3:                                                ; preds = %label_entry, %label58
  %op4 = load i32, i32* %op0
  %op5 = icmp slt i32 %op4, 10000
  %op6 = zext i1 %op5 to i32
  %op7 = icmp ne i32 %op6, 0
  br i1 %op7, label %label8, label %label9
label8:                                                ; preds = %label3
  store i32 0, i32* %op1
  br label %label11
label9:                                                ; preds = %label3
  %op10 = load i32, i32* %op2
  call void @output(i32 %op10)
  ret void
label11:                                                ; preds = %label8, %label16
  %op12 = load i32, i32* %op1
  %op13 = icmp slt i32 %op12, 10000
  %op14 = zext i1 %op13 to i32
  %op15 = icmp ne i32 %op14, 0
  br i1 %op15, label %label16, label %label58
label16:                                                ; preds = %label11
  %op17 = load i32, i32* %op0
  %op18 = load i32, i32* %op0
  %op19 = mul i32 %op17, %op18
  %op20 = load i32, i32* %op0
  %op21 = mul i32 %op19, %op20
  %op22 = load i32, i32* %op0
  %op23 = mul i32 %op21, %op22
  %op24 = load i32, i32* %op0
  %op25 = mul i32 %op23, %op24
  %op26 = load i32, i32* %op0
  %op27 = mul i32 %op25, %op26
  %op28 = load i32, i32* %op0
  %op29 = mul i32 %op27, %op28
  %op30 = load i32, i32* %op0
  %op31 = mul i32 %op29, %op30
  %op32 = load i32, i32* %op0
  %op33 = mul i32 %op31, %op32
  %op34 = load i32, i32* %op0
  %op35 = mul i32 %op33, %op34
  %op36 = load i32, i32* %op0
  %op37 = sdiv i32 %op35, %op36
  %op38 = load i32, i32* %op0
  %op39 = sdiv i32 %op37, %op38
  %op40 = load i32, i32* %op0
  %op41 = sdiv i32 %op39, %op40
  %op42 = load i32, i32* %op0
  %op43 = sdiv i32 %op41, %op42
  %op44 = load i32, i32* %op0
  %op45 = sdiv i32 %op43, %op44
  %op46 = load i32, i32* %op0
  %op47 = sdiv i32 %op45, %op46
  %op48 = load i32, i32* %op0
  %op49 = sdiv i32 %op47, %op48
  %op50 = load i32, i32* %op0
  %op51 = sdiv i32 %op49, %op50
  %op52 = load i32, i32* %op0
  %op53 = sdiv i32 %op51, %op52
  %op54 = load i32, i32* %op0
  %op55 = sdiv i32 %op53, %op54
  store i32 %op55, i32* %op2
  %op56 = load i32, i32* %op1
  %op57 = add i32 %op56, 1
  store i32 %op57, i32* %op1
  br label %label11
label58:                                                ; preds = %label11
  %op59 = load i32, i32* %op0
  %op60 = add i32 %op59, 1
  store i32 %op60, i32* %op0
  br label %label3
}
