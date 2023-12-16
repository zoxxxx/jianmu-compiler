; ModuleID = 'cminus'
source_filename = "/home/zox/compiler/2023ustc-jianmu-compiler/tests/4-mem2reg/functional-cases/7-function.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @min(i32 %arg0, i32 %arg1) {
label_entry:
  %op2 = icmp sle i32 %arg0, %arg1
  br i1 %op2, label %label3, label %label4
label3:                                                ; preds = %label_entry
  ret i32 %arg0
label4:                                                ; preds = %label_entry
  ret i32 %arg1
label5:
  ret i32 0
}
define i32 @main() {
label_entry:
  %op0 = call i32 @min(i32 11, i32 22)
  call void @output(i32 %op0)
  %op1 = call i32 @min(i32 22, i32 33)
  call void @output(i32 %op1)
  %op2 = call i32 @min(i32 33, i32 11)
  call void @output(i32 %op2)
  ret i32 0
}
