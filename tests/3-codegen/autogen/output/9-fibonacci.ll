; ModuleID = 'cminus'
source_filename = "/home/zox/compiler/2023ustc-jianmu-compiler/tests/3-codegen/autogen/testcases/9-fibonacci.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @fibonacci(i32 %arg0) {
label_entry:
  %op1 = alloca i32
  store i32 %arg0, i32* %op1
  %op2 = load i32, i32* %op1
  %op3 = icmp eq i32 %op2, 0
  br i1 %op3, label %label4, label %label5
label4:                                                ; preds = %label_entry
  ret i32 0
label5:                                                ; preds = %label_entry
  %op6 = load i32, i32* %op1
  %op7 = icmp eq i32 %op6, 1
  br i1 %op7, label %label9, label %label10
label8:                                                ; preds = %label18
  ret i32 0
label9:                                                ; preds = %label5
  ret i32 1
label10:                                                ; preds = %label5
  %op11 = load i32, i32* %op1
  %op12 = sub i32 %op11, 1
  %op13 = call i32 @fibonacci(i32 %op12)
  %op14 = load i32, i32* %op1
  %op15 = sub i32 %op14, 2
  %op16 = call i32 @fibonacci(i32 %op15)
  %op17 = add i32 %op13, %op16
  ret i32 %op17
label18:
  br label %label8
}
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
  br i1 %op5, label %label6, label %label11
label6:                                                ; preds = %label2
  %op7 = load i32, i32* %op1
  %op8 = call i32 @fibonacci(i32 %op7)
  call void @output(i32 %op8)
  %op9 = load i32, i32* %op1
  %op10 = add i32 %op9, 1
  store i32 %op10, i32* %op1
  br label %label2
label11:                                                ; preds = %label2
  ret i32 0
}
