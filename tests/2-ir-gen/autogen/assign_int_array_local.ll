; ModuleID = 'cminus'
source_filename = "/home/zox/compiler/2023ustc-jianmu-compiler/tests/2-ir-gen/autogen/testcases/lv1/assign_int_array_local.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @main() {
label0:
  %op1 = alloca [10 x i32]
  %op2 = icmp slt i32 3, 0
  br i1 %op2, label %label3, label %label4
label3:                                                ; preds = %label0
  call void @neg_idx_except()
label4:                                                ; preds = %label0
  %op5 = getelementptr [10 x i32], [10 x i32]* %op1, i32 0, i32 3
  store i32 1234, i32* %op5
  %op6 = icmp slt i32 3, 0
  br i1 %op6, label %label7, label %label8
label7:                                                ; preds = %label4
  call void @neg_idx_except()
label8:                                                ; preds = %label4
  %op9 = getelementptr [10 x i32], [10 x i32]* %op1, i32 0, i32 3
  %op10 = load i32, i32* %op9
  call void @output(i32 %op10)
  ret void
}
