; ModuleID = 'cminus'
source_filename = "/home/zox/compiler/2023ustc-jianmu-compiler/tests/2-ir-gen/autogen/testcases/lv3/complex1.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @mod(i32 %arg0, i32 %arg1) {
label_entry:
  %op2 = alloca i32
  store i32 %arg0, i32* %op2
  %op3 = alloca i32
  store i32 %arg1, i32* %op3
  %op4 = load i32, i32* %op2
  %op5 = load i32, i32* %op2
  %op6 = load i32, i32* %op3
  %op7 = sdiv i32 %op5, %op6
  %op8 = load i32, i32* %op3
  %op9 = mul i32 %op7, %op8
  %op10 = sub i32 %op4, %op9
  ret i32 %op10
}
define void @printfour(i32 %arg0) {
label_entry:
  %op1 = alloca i32
  store i32 %arg0, i32* %op1
  %op2 = alloca i32
  %op3 = alloca i32
  %op4 = alloca i32
  %op5 = alloca i32
  %op6 = load i32, i32* %op1
  %op7 = call i32 @mod(i32 %op6, i32 10000)
  store i32 %op7, i32* %op1
  %op8 = load i32, i32* %op1
  %op9 = call i32 @mod(i32 %op8, i32 10)
  store i32 %op9, i32* %op5
  %op10 = load i32, i32* %op1
  %op11 = sdiv i32 %op10, 10
  store i32 %op11, i32* %op1
  %op12 = load i32, i32* %op1
  %op13 = call i32 @mod(i32 %op12, i32 10)
  store i32 %op13, i32* %op4
  %op14 = load i32, i32* %op1
  %op15 = sdiv i32 %op14, 10
  store i32 %op15, i32* %op1
  %op16 = load i32, i32* %op1
  %op17 = call i32 @mod(i32 %op16, i32 10)
  store i32 %op17, i32* %op3
  %op18 = load i32, i32* %op1
  %op19 = sdiv i32 %op18, 10
  store i32 %op19, i32* %op1
  %op20 = load i32, i32* %op1
  store i32 %op20, i32* %op2
  %op21 = load i32, i32* %op2
  call void @output(i32 %op21)
  %op22 = load i32, i32* %op3
  call void @output(i32 %op22)
  %op23 = load i32, i32* %op4
  call void @output(i32 %op23)
  %op24 = load i32, i32* %op5
  call void @output(i32 %op24)
  ret void
}
define void @main() {
label_entry:
  %op0 = alloca [2801 x i32]
  %op1 = alloca i32
  %op2 = alloca i32
  %op3 = alloca i32
  %op4 = alloca i32
  %op5 = alloca i32
  store i32 0, i32* %op5
  store i32 1234, i32* %op4
  %op6 = alloca i32
  store i32 0, i32* %op6
  br label %label7
label7:                                                ; preds = %label_entry, %label15
  %op8 = load i32, i32* %op6
  %op9 = icmp slt i32 %op8, 2800
  br i1 %op9, label %label10, label %label13
label10:                                                ; preds = %label7
  %op11 = load i32, i32* %op6
  %op12 = icmp slt i32 %op11, 0
  br i1 %op12, label %label14, label %label15
label13:                                                ; preds = %label7
  store i32 2800, i32* %op2
  br label %label19
label14:                                                ; preds = %label10
  call void @neg_idx_except()
  br label %label15
label15:                                                ; preds = %label10, %label14
  %op16 = getelementptr [2801 x i32], [2801 x i32]* %op0, i32 0, i32 %op11
  store i32 2000, i32* %op16
  %op17 = load i32, i32* %op6
  %op18 = add i32 %op17, 1
  store i32 %op18, i32* %op6
  br label %label7
label19:                                                ; preds = %label13, %label33
  %op20 = load i32, i32* %op2
  %op21 = icmp ne i32 %op20, 0
  br i1 %op21, label %label22, label %label25
label22:                                                ; preds = %label19
  %op23 = alloca i32
  store i32 0, i32* %op23
  %op24 = load i32, i32* %op2
  store i32 %op24, i32* %op1
  br label %label26
label25:                                                ; preds = %label19
  ret void
label26:                                                ; preds = %label22, %label70
  %op27 = load i32, i32* %op1
  %op28 = icmp ne i32 %op27, 0
  br i1 %op28, label %label29, label %label33
label29:                                                ; preds = %label26
  %op30 = load i32, i32* %op23
  %op31 = load i32, i32* %op1
  %op32 = icmp slt i32 %op31, 0
  br i1 %op32, label %label42, label %label43
label33:                                                ; preds = %label26
  %op34 = load i32, i32* %op5
  %op35 = load i32, i32* %op23
  %op36 = sdiv i32 %op35, 10000
  %op37 = add i32 %op34, %op36
  call void @printfour(i32 %op37)
  %op38 = load i32, i32* %op23
  %op39 = call i32 @mod(i32 %op38, i32 10000)
  store i32 %op39, i32* %op5
  %op40 = load i32, i32* %op2
  %op41 = sub i32 %op40, 14
  store i32 %op41, i32* %op2
  br label %label19
label42:                                                ; preds = %label29
  call void @neg_idx_except()
  br label %label43
label43:                                                ; preds = %label29, %label42
  %op44 = getelementptr [2801 x i32], [2801 x i32]* %op0, i32 0, i32 %op31
  %op45 = load i32, i32* %op44
  %op46 = mul i32 %op45, 10000
  %op47 = add i32 %op30, %op46
  store i32 %op47, i32* %op23
  %op48 = load i32, i32* %op1
  %op49 = mul i32 2, %op48
  %op50 = sub i32 %op49, 1
  store i32 %op50, i32* %op3
  %op51 = load i32, i32* %op1
  %op52 = icmp slt i32 %op51, 0
  br i1 %op52, label %label53, label %label54
label53:                                                ; preds = %label43
  call void @neg_idx_except()
  br label %label54
label54:                                                ; preds = %label43, %label53
  %op55 = getelementptr [2801 x i32], [2801 x i32]* %op0, i32 0, i32 %op51
  %op56 = load i32, i32* %op23
  %op57 = load i32, i32* %op3
  %op58 = call i32 @mod(i32 %op56, i32 %op57)
  store i32 %op58, i32* %op55
  %op59 = load i32, i32* %op23
  %op60 = load i32, i32* %op3
  %op61 = sdiv i32 %op59, %op60
  store i32 %op61, i32* %op23
  %op62 = load i32, i32* %op1
  %op63 = sub i32 %op62, 1
  store i32 %op63, i32* %op1
  %op64 = load i32, i32* %op1
  %op65 = icmp ne i32 %op64, 0
  br i1 %op65, label %label66, label %label70
label66:                                                ; preds = %label54
  %op67 = load i32, i32* %op23
  %op68 = load i32, i32* %op1
  %op69 = mul i32 %op67, %op68
  store i32 %op69, i32* %op23
  br label %label70
label70:                                                ; preds = %label54, %label66
  br label %label26
}
