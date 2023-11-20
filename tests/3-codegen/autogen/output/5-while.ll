; ModuleID = 'cminus'
source_filename = "/home/zox/compiler/2023ustc-jianmu-compiler/tests/3-codegen/autogen/testcases/5-while.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @main() {
label_entry:
  %op0 = alloca i32
  %op1 = alloca i32
  store i32 10, i32* %op0
  store i32 0, i32* %op1
  br label %label2
label2:                                                ; preds = %label_entry, %label6
  %op3 = load i32, i32* %op1
  %op4 = load i32, i32* %op0
  %op5 = icmp slt i32 %op3, %op4
  br i1 %op5, label %label6, label %label10
label6:                                                ; preds = %label2
  %op7 = load i32, i32* %op1
  call void @output(i32 %op7)
  %op8 = load i32, i32* %op1
  %op9 = add i32 %op8, 1
  store i32 %op9, i32* %op1
  br label %label2
label10:                                                ; preds = %label2
  ret i32 0
}
