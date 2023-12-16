; ModuleID = 'cminus'
source_filename = "/home/zox/compiler/2023ustc-jianmu-compiler/tests/4-mem2reg/functional-cases/hack-2.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @main() {
label_entry:
  br label %label0
label0:                                                ; preds = %label_entry, %label5
  %op1 = phi i32 [ 0, %label_entry ], [ %op2, %label5 ]
  %op2 = phi i32 [ 0, %label_entry ], [ %op7, %label5 ]
  %op3 = phi i32 [ 0, %label_entry ], [ %op6, %label5 ]
  %op4 = icmp sle i32 %op3, 1
  br i1 %op4, label %label5, label %label8
label5:                                                ; preds = %label0
  %op6 = add i32 %op3, 1
  %op7 = add i32 %op2, 1
  br label %label0
label8:                                                ; preds = %label0
  call void @output(i32 %op1)
  ret i32 0
}
