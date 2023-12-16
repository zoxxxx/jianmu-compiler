; ModuleID = 'cminus'
source_filename = "/home/zox/compiler/2023ustc-jianmu-compiler/tests/4-mem2reg/functional-cases/13-complex.cminus"

@n = global i32 zeroinitializer
@m = global i32 zeroinitializer
@w = global [5 x i32] zeroinitializer
@v = global [5 x i32] zeroinitializer
@dp = global [66 x i32] zeroinitializer
declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @max(i32 %arg0, i32 %arg1) {
label_entry:
  %op2 = icmp sgt i32 %arg0, %arg1
  br i1 %op2, label %label3, label %label4
label3:                                                ; preds = %label_entry
  ret i32 %arg0
label4:                                                ; preds = %label_entry
  ret i32 %arg1
label5:
  ret i32 0
}
define i32 @knapsack(i32 %arg0, i32 %arg1) {
label_entry:
  %op2 = icmp sle i32 %arg1, 0
  br i1 %op2, label %label3, label %label4
label3:                                                ; preds = %label_entry
  ret i32 0
label4:                                                ; preds = %label_entry
  %op5 = icmp eq i32 %arg0, 0
  br i1 %op5, label %label6, label %label7
label6:                                                ; preds = %label4
  ret i32 0
label7:                                                ; preds = %label4
  %op8 = mul i32 %arg0, 11
  %op9 = add i32 %op8, %arg1
  %op10 = icmp slt i32 %op9, 0
  br i1 %op10, label %label11, label %label12
label11:                                                ; preds = %label7
  call void @neg_idx_except()
  br label %label12
label12:                                                ; preds = %label7, %label11
  %op13 = getelementptr [66 x i32], [66 x i32]* @dp, i32 0, i32 %op9
  %op14 = load i32, i32* %op13
  %op15 = icmp sge i32 %op14, 0
  br i1 %op15, label %label16, label %label20
label16:                                                ; preds = %label12
  %op17 = mul i32 %arg0, 11
  %op18 = add i32 %op17, %arg1
  %op19 = icmp slt i32 %op18, 0
  br i1 %op19, label %label23, label %label24
label20:                                                ; preds = %label12
  %op21 = sub i32 %arg0, 1
  %op22 = icmp slt i32 %op21, 0
  br i1 %op22, label %label27, label %label28
label23:                                                ; preds = %label16
  call void @neg_idx_except()
  br label %label24
label24:                                                ; preds = %label16, %label23
  %op25 = getelementptr [66 x i32], [66 x i32]* @dp, i32 0, i32 %op18
  %op26 = load i32, i32* %op25
  ret i32 %op26
label27:                                                ; preds = %label20
  call void @neg_idx_except()
  br label %label28
label28:                                                ; preds = %label20, %label27
  %op29 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 %op21
  %op30 = load i32, i32* %op29
  %op31 = icmp slt i32 %arg1, %op30
  br i1 %op31, label %label32, label %label35
label32:                                                ; preds = %label28
  %op33 = sub i32 %arg0, 1
  %op34 = call i32 @knapsack(i32 %op33, i32 %arg1)
  br label %label41
label35:                                                ; preds = %label28
  %op36 = sub i32 %arg0, 1
  %op37 = call i32 @knapsack(i32 %op36, i32 %arg1)
  %op38 = sub i32 %arg0, 1
  %op39 = sub i32 %arg0, 1
  %op40 = icmp slt i32 %op39, 0
  br i1 %op40, label %label46, label %label47
label41:                                                ; preds = %label32, %label55
  %op42 = phi i32 [ %op34, %label32 ], [ %op59, %label55 ]
  %op43 = mul i32 %arg0, 11
  %op44 = add i32 %op43, %arg1
  %op45 = icmp slt i32 %op44, 0
  br i1 %op45, label %label60, label %label61
label46:                                                ; preds = %label35
  call void @neg_idx_except()
  br label %label47
label47:                                                ; preds = %label35, %label46
  %op48 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 %op39
  %op49 = load i32, i32* %op48
  %op50 = sub i32 %arg1, %op49
  %op51 = call i32 @knapsack(i32 %op38, i32 %op50)
  %op52 = sub i32 %arg0, 1
  %op53 = icmp slt i32 %op52, 0
  br i1 %op53, label %label54, label %label55
label54:                                                ; preds = %label47
  call void @neg_idx_except()
  br label %label55
label55:                                                ; preds = %label47, %label54
  %op56 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 %op52
  %op57 = load i32, i32* %op56
  %op58 = add i32 %op51, %op57
  %op59 = call i32 @max(i32 %op37, i32 %op58)
  br label %label41
label60:                                                ; preds = %label41
  call void @neg_idx_except()
  br label %label61
label61:                                                ; preds = %label41, %label60
  %op62 = getelementptr [66 x i32], [66 x i32]* @dp, i32 0, i32 %op44
  store i32 %op42, i32* %op62
  ret i32 %op42
}
define i32 @main() {
label_entry:
  store i32 5, i32* @n
  store i32 10, i32* @m
  %op0 = icmp slt i32 0, 0
  br i1 %op0, label %label1, label %label2
label1:                                                ; preds = %label_entry
  call void @neg_idx_except()
  br label %label2
label2:                                                ; preds = %label_entry, %label1
  %op3 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 0
  store i32 2, i32* %op3
  %op4 = icmp slt i32 1, 0
  br i1 %op4, label %label5, label %label6
label5:                                                ; preds = %label2
  call void @neg_idx_except()
  br label %label6
label6:                                                ; preds = %label2, %label5
  %op7 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 1
  store i32 2, i32* %op7
  %op8 = icmp slt i32 2, 0
  br i1 %op8, label %label9, label %label10
label9:                                                ; preds = %label6
  call void @neg_idx_except()
  br label %label10
label10:                                                ; preds = %label6, %label9
  %op11 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 2
  store i32 6, i32* %op11
  %op12 = icmp slt i32 3, 0
  br i1 %op12, label %label13, label %label14
label13:                                                ; preds = %label10
  call void @neg_idx_except()
  br label %label14
label14:                                                ; preds = %label10, %label13
  %op15 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 3
  store i32 5, i32* %op15
  %op16 = icmp slt i32 4, 0
  br i1 %op16, label %label17, label %label18
label17:                                                ; preds = %label14
  call void @neg_idx_except()
  br label %label18
label18:                                                ; preds = %label14, %label17
  %op19 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 4
  store i32 4, i32* %op19
  %op20 = icmp slt i32 0, 0
  br i1 %op20, label %label21, label %label22
label21:                                                ; preds = %label18
  call void @neg_idx_except()
  br label %label22
label22:                                                ; preds = %label18, %label21
  %op23 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 0
  store i32 6, i32* %op23
  %op24 = icmp slt i32 1, 0
  br i1 %op24, label %label25, label %label26
label25:                                                ; preds = %label22
  call void @neg_idx_except()
  br label %label26
label26:                                                ; preds = %label22, %label25
  %op27 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 1
  store i32 3, i32* %op27
  %op28 = icmp slt i32 2, 0
  br i1 %op28, label %label29, label %label30
label29:                                                ; preds = %label26
  call void @neg_idx_except()
  br label %label30
label30:                                                ; preds = %label26, %label29
  %op31 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 2
  store i32 5, i32* %op31
  %op32 = icmp slt i32 3, 0
  br i1 %op32, label %label33, label %label34
label33:                                                ; preds = %label30
  call void @neg_idx_except()
  br label %label34
label34:                                                ; preds = %label30, %label33
  %op35 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 3
  store i32 4, i32* %op35
  %op36 = icmp slt i32 4, 0
  br i1 %op36, label %label37, label %label38
label37:                                                ; preds = %label34
  call void @neg_idx_except()
  br label %label38
label38:                                                ; preds = %label34, %label37
  %op39 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 4
  store i32 6, i32* %op39
  br label %label40
label40:                                                ; preds = %label38, %label50
  %op41 = phi i32 [ 0, %label38 ], [ %op53, %label50 ]
  %op42 = icmp slt i32 %op41, 66
  br i1 %op42, label %label43, label %label45
label43:                                                ; preds = %label40
  %op44 = icmp slt i32 %op41, 0
  br i1 %op44, label %label49, label %label50
label45:                                                ; preds = %label40
  %op46 = load i32, i32* @n
  %op47 = load i32, i32* @m
  %op48 = call i32 @knapsack(i32 %op46, i32 %op47)
  call void @output(i32 %op48)
  ret i32 0
label49:                                                ; preds = %label43
  call void @neg_idx_except()
  br label %label50
label50:                                                ; preds = %label43, %label49
  %op51 = getelementptr [66 x i32], [66 x i32]* @dp, i32 0, i32 %op41
  %op52 = sub i32 0, 1
  store i32 %op52, i32* %op51
  %op53 = add i32 %op41, 1
  br label %label40
}
