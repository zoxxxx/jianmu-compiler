; ModuleID = 'cminus'
source_filename = "/home/zox/compiler/2023ustc-jianmu-compiler/tests/3-codegen/autogen/testcases/12-global.cminus"

@seed = global i32 zeroinitializer
declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @randomLCG() {
label_entry:
  %op0 = load i32, i32* @seed
  %op1 = mul i32 %op0, 1103515245
  %op2 = add i32 %op1, 12345
  store i32 %op2, i32* @seed
  %op3 = load i32, i32* @seed
  ret i32 %op3
}
define i32 @randBin() {
label_entry:
  %op0 = call i32 @randomLCG()
  %op1 = icmp sgt i32 %op0, 0
  br i1 %op1, label %label2, label %label3
label2:                                                ; preds = %label_entry
  ret i32 1
label3:                                                ; preds = %label_entry
  ret i32 0
label4:
  ret i32 0
}
define i32 @returnToZeroSteps() {
label_entry:
  %op0 = alloca i32
  %op1 = alloca i32
  store i32 0, i32* %op0
  store i32 0, i32* %op1
  br label %label2
label2:                                                ; preds = %label_entry, %label22
  %op3 = load i32, i32* %op1
  %op4 = icmp slt i32 %op3, 20
  br i1 %op4, label %label5, label %label8
label5:                                                ; preds = %label2
  %op6 = call i32 @randBin()
  %op7 = icmp ne i32 %op6, 0
  br i1 %op7, label %label9, label %label12
label8:                                                ; preds = %label2
  ret i32 20
label9:                                                ; preds = %label5
  %op10 = load i32, i32* %op0
  %op11 = add i32 %op10, 1
  store i32 %op11, i32* %op0
  br label %label15
label12:                                                ; preds = %label5
  %op13 = load i32, i32* %op0
  %op14 = sub i32 %op13, 1
  store i32 %op14, i32* %op0
  br label %label15
label15:                                                ; preds = %label9, %label12
  %op16 = load i32, i32* %op1
  %op17 = add i32 %op16, 1
  store i32 %op17, i32* %op1
  %op18 = load i32, i32* %op0
  %op19 = icmp eq i32 %op18, 0
  br i1 %op19, label %label20, label %label22
label20:                                                ; preds = %label15
  %op21 = load i32, i32* %op1
  ret i32 %op21
label22:                                                ; preds = %label15
  br label %label2
}
define i32 @main() {
label_entry:
  %op0 = alloca i32
  store i32 0, i32* %op0
  store i32 3407, i32* @seed
  br label %label1
label1:                                                ; preds = %label_entry, %label4
  %op2 = load i32, i32* %op0
  %op3 = icmp slt i32 %op2, 20
  br i1 %op3, label %label4, label %label8
label4:                                                ; preds = %label1
  %op5 = call i32 @returnToZeroSteps()
  call void @output(i32 %op5)
  %op6 = load i32, i32* %op0
  %op7 = add i32 %op6, 1
  store i32 %op7, i32* %op0
  br label %label1
label8:                                                ; preds = %label1
  ret i32 0
}
