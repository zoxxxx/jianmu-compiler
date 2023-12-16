; ModuleID = 'cminus'
source_filename = "/home/zox/compiler/2023ustc-jianmu-compiler/tests/4-mem2reg/functional-cases/5-while.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @main() {
label_entry:
  br label %label0
label0:                                                ; preds = %label_entry, %label3
  %op1 = phi i32 [ 0, %label_entry ], [ %op4, %label3 ]
  %op2 = icmp slt i32 %op1, 10
  br i1 %op2, label %label3, label %label5
label3:                                                ; preds = %label0
  call void @output(i32 %op1)
  %op4 = add i32 %op1, 1
  br label %label0
label5:                                                ; preds = %label0
  ret i32 0
}
