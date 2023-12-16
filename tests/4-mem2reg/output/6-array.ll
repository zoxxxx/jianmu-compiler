; ModuleID = 'cminus'
source_filename = "/home/zox/compiler/2023ustc-jianmu-compiler/tests/4-mem2reg/functional-cases/6-array.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @main() {
label_entry:
  %op0 = alloca [10 x i32]
  %op1 = icmp slt i32 0, 0
  br i1 %op1, label %label2, label %label3
label2:                                                ; preds = %label_entry
  call void @neg_idx_except()
  br label %label3
label3:                                                ; preds = %label_entry, %label2
  %op4 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 0
  store i32 11, i32* %op4
  %op5 = icmp slt i32 4, 0
  br i1 %op5, label %label6, label %label7
label6:                                                ; preds = %label3
  call void @neg_idx_except()
  br label %label7
label7:                                                ; preds = %label3, %label6
  %op8 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 4
  store i32 22, i32* %op8
  %op9 = icmp slt i32 9, 0
  br i1 %op9, label %label10, label %label11
label10:                                                ; preds = %label7
  call void @neg_idx_except()
  br label %label11
label11:                                                ; preds = %label7, %label10
  %op12 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 9
  store i32 33, i32* %op12
  %op13 = icmp slt i32 0, 0
  br i1 %op13, label %label14, label %label15
label14:                                                ; preds = %label11
  call void @neg_idx_except()
  br label %label15
label15:                                                ; preds = %label11, %label14
  %op16 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 0
  %op17 = load i32, i32* %op16
  call void @output(i32 %op17)
  %op18 = icmp slt i32 4, 0
  br i1 %op18, label %label19, label %label20
label19:                                                ; preds = %label15
  call void @neg_idx_except()
  br label %label20
label20:                                                ; preds = %label15, %label19
  %op21 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 4
  %op22 = load i32, i32* %op21
  call void @output(i32 %op22)
  %op23 = icmp slt i32 9, 0
  br i1 %op23, label %label24, label %label25
label24:                                                ; preds = %label20
  call void @neg_idx_except()
  br label %label25
label25:                                                ; preds = %label20, %label24
  %op26 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 9
  %op27 = load i32, i32* %op26
  call void @output(i32 %op27)
  ret i32 0
}
