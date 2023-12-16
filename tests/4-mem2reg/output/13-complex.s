# Global variables
	.text
	.section .bss, "aw", @nobits
	.globl n
	.type n, @object
	.size n, 4
n:
	.space 4
	.globl m
	.type m, @object
	.size m, 4
m:
	.space 4
	.globl w
	.type w, @object
	.size w, 20
w:
	.space 20
	.globl v
	.type v, @object
	.size v, 20
v:
	.space 20
	.globl dp
	.type dp, @object
	.size dp, 264
dp:
	.space 264
	.text
	.globl max
	.type max, @function
max:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -32
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
.max_label_entry:
# %op2 = icmp sgt i32 %arg0, %arg1
	ld.w $t0, $fp, -20
	ld.w $t1, $fp, -24
	slt $t2, $t1, $t0
	st.b $t2, $fp, -25
# br i1 %op2, label %label3, label %label4
	ld.b $t0, $fp, -25
	bnez $t0, .max_label3
	b .max_label4
.max_label3:
# ret i32 %arg0
	ld.w $a0, $fp, -20
	b max_exit
.max_label4:
# ret i32 %arg1
	ld.w $a0, $fp, -24
	b max_exit
.max_label5:
# ret i32 0
	addi.w $a0, $zero, 0
	b max_exit
max_exit:
	addi.d $sp, $sp, 32
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl knapsack
	.type knapsack, @function
knapsack:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -224
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
.knapsack_label_entry:
# %op2 = icmp sle i32 %arg1, 0
	ld.w $t0, $fp, -24
	addi.w $t1, $zero, 0
	slt $t2, $t1, $t0
	xori $t2, $t2, 1
	st.b $t2, $fp, -25
# br i1 %op2, label %label3, label %label4
	ld.b $t0, $fp, -25
	bnez $t0, .knapsack_label3
	b .knapsack_label4
.knapsack_label3:
# ret i32 0
	addi.w $a0, $zero, 0
	b knapsack_exit
.knapsack_label4:
# %op5 = icmp eq i32 %arg0, 0
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	slt $t3, $t1, $t0
	or $t2, $t2, $t3
	xori $t2, $t2, 1
	st.b $t2, $fp, -26
# br i1 %op5, label %label6, label %label7
	ld.b $t0, $fp, -26
	bnez $t0, .knapsack_label6
	b .knapsack_label7
.knapsack_label6:
# ret i32 0
	addi.w $a0, $zero, 0
	b knapsack_exit
.knapsack_label7:
# %op8 = mul i32 %arg0, 11
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 11
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -32
# %op9 = add i32 %op8, %arg1
	ld.w $t0, $fp, -32
	ld.w $t1, $fp, -24
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -36
# %op10 = icmp slt i32 %op9, 0
	ld.w $t0, $fp, -36
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -37
# br i1 %op10, label %label11, label %label12
	ld.b $t0, $fp, -37
	bnez $t0, .knapsack_label11
	b .knapsack_label12
.knapsack_label11:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label12
	b .knapsack_label12
.knapsack_label12:
# %op13 = getelementptr [66 x i32], [66 x i32]* @dp, i32 0, i32 %op9
	la.local $t0, dp
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -36
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -48
# %op14 = load i32, i32* %op13
	ld.d $t0, $fp, -48
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -52
# %op15 = icmp sge i32 %op14, 0
	ld.w $t0, $fp, -52
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	xori $t2, $t2, 1
	st.b $t2, $fp, -53
# br i1 %op15, label %label16, label %label20
	ld.b $t0, $fp, -53
	bnez $t0, .knapsack_label16
	b .knapsack_label20
.knapsack_label16:
# %op17 = mul i32 %arg0, 11
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 11
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -60
# %op18 = add i32 %op17, %arg1
	ld.w $t0, $fp, -60
	ld.w $t1, $fp, -24
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -64
# %op19 = icmp slt i32 %op18, 0
	ld.w $t0, $fp, -64
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -65
# br i1 %op19, label %label23, label %label24
	ld.b $t0, $fp, -65
	bnez $t0, .knapsack_label23
	b .knapsack_label24
.knapsack_label20:
# %op21 = sub i32 %arg0, 1
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -72
# %op22 = icmp slt i32 %op21, 0
	ld.w $t0, $fp, -72
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -73
# br i1 %op22, label %label27, label %label28
	ld.b $t0, $fp, -73
	bnez $t0, .knapsack_label27
	b .knapsack_label28
.knapsack_label23:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label24
	b .knapsack_label24
.knapsack_label24:
# %op25 = getelementptr [66 x i32], [66 x i32]* @dp, i32 0, i32 %op18
	la.local $t0, dp
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -64
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -88
# %op26 = load i32, i32* %op25
	ld.d $t0, $fp, -88
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -92
# ret i32 %op26
	ld.w $a0, $fp, -92
	b knapsack_exit
.knapsack_label27:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label28
	b .knapsack_label28
.knapsack_label28:
# %op29 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 %op21
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -72
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -104
# %op30 = load i32, i32* %op29
	ld.d $t0, $fp, -104
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -108
# %op31 = icmp slt i32 %arg1, %op30
	ld.w $t0, $fp, -24
	ld.w $t1, $fp, -108
	slt $t2, $t0, $t1
	st.b $t2, $fp, -109
# br i1 %op31, label %label32, label %label35
	ld.b $t0, $fp, -109
	bnez $t0, .knapsack_label32
	b .knapsack_label35
.knapsack_label32:
# %op33 = sub i32 %arg0, 1
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -116
# %op34 = call i32 @knapsack(i32 %op33, i32 %arg1)
	ld.w $a0, $fp, -116
	ld.w $a1, $fp, -24
	bl knapsack
	st.w $a0, $fp, -120
# br label %label41
	ld.w $t0, $fp, -120
	st.w $t0, $fp, -144
	b .knapsack_label41
.knapsack_label35:
# %op36 = sub i32 %arg0, 1
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -124
# %op37 = call i32 @knapsack(i32 %op36, i32 %arg1)
	ld.w $a0, $fp, -124
	ld.w $a1, $fp, -24
	bl knapsack
	st.w $a0, $fp, -128
# %op38 = sub i32 %arg0, 1
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -132
# %op39 = sub i32 %arg0, 1
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -136
# %op40 = icmp slt i32 %op39, 0
	ld.w $t0, $fp, -136
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -137
# br i1 %op40, label %label46, label %label47
	ld.b $t0, $fp, -137
	bnez $t0, .knapsack_label46
	b .knapsack_label47
.knapsack_label41:
# %op42 = phi i32 [ %op34, %label32 ], [ %op59, %label55 ]
# %op43 = mul i32 %arg0, 11
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 11
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -148
# %op44 = add i32 %op43, %arg1
	ld.w $t0, $fp, -148
	ld.w $t1, $fp, -24
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -152
# %op45 = icmp slt i32 %op44, 0
	ld.w $t0, $fp, -152
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -153
# br i1 %op45, label %label60, label %label61
	ld.b $t0, $fp, -153
	bnez $t0, .knapsack_label60
	b .knapsack_label61
.knapsack_label46:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label47
	b .knapsack_label47
.knapsack_label47:
# %op48 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 %op39
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -136
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -168
# %op49 = load i32, i32* %op48
	ld.d $t0, $fp, -168
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -172
# %op50 = sub i32 %arg1, %op49
	ld.w $t0, $fp, -24
	ld.w $t1, $fp, -172
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -176
# %op51 = call i32 @knapsack(i32 %op38, i32 %op50)
	ld.w $a0, $fp, -132
	ld.w $a1, $fp, -176
	bl knapsack
	st.w $a0, $fp, -180
# %op52 = sub i32 %arg0, 1
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -184
# %op53 = icmp slt i32 %op52, 0
	ld.w $t0, $fp, -184
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -185
# br i1 %op53, label %label54, label %label55
	ld.b $t0, $fp, -185
	bnez $t0, .knapsack_label54
	b .knapsack_label55
.knapsack_label54:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label55
	b .knapsack_label55
.knapsack_label55:
# %op56 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 %op52
	la.local $t0, v
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -184
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -200
# %op57 = load i32, i32* %op56
	ld.d $t0, $fp, -200
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -204
# %op58 = add i32 %op51, %op57
	ld.w $t0, $fp, -180
	ld.w $t1, $fp, -204
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -208
# %op59 = call i32 @max(i32 %op37, i32 %op58)
	ld.w $a0, $fp, -128
	ld.w $a1, $fp, -208
	bl max
	st.w $a0, $fp, -212
# br label %label41
	ld.w $t0, $fp, -212
	st.w $t0, $fp, -144
	b .knapsack_label41
.knapsack_label60:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label61
	b .knapsack_label61
.knapsack_label61:
# %op62 = getelementptr [66 x i32], [66 x i32]* @dp, i32 0, i32 %op44
	la.local $t0, dp
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -152
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -224
# store i32 %op42, i32* %op62
	ld.d $t0, $fp, -224
	ld.w $t1, $fp, -144
	st.w $t1, $t0, 0
# ret i32 %op42
	ld.w $a0, $fp, -144
	b knapsack_exit
knapsack_exit:
	addi.d $sp, $sp, 224
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -224
.main_label_entry:
# store i32 5, i32* @n
	la.local $t0, n
	addi.w $t1, $zero, 5
	st.w $t1, $t0, 0
# store i32 10, i32* @m
	la.local $t0, m
	addi.w $t1, $zero, 10
	st.w $t1, $t0, 0
# %op0 = icmp slt i32 0, 0
	addi.w $t0, $zero, 0
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -17
# br i1 %op0, label %label1, label %label2
	ld.b $t0, $fp, -17
	bnez $t0, .main_label1
	b .main_label2
.main_label1:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label2
	b .main_label2
.main_label2:
# %op3 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 0
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -32
# store i32 2, i32* %op3
	ld.d $t0, $fp, -32
	addi.w $t1, $zero, 2
	st.w $t1, $t0, 0
# %op4 = icmp slt i32 1, 0
	addi.w $t0, $zero, 1
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -33
# br i1 %op4, label %label5, label %label6
	ld.b $t0, $fp, -33
	bnez $t0, .main_label5
	b .main_label6
.main_label5:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label6
	b .main_label6
.main_label6:
# %op7 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 1
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 1
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -48
# store i32 2, i32* %op7
	ld.d $t0, $fp, -48
	addi.w $t1, $zero, 2
	st.w $t1, $t0, 0
# %op8 = icmp slt i32 2, 0
	addi.w $t0, $zero, 2
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -49
# br i1 %op8, label %label9, label %label10
	ld.b $t0, $fp, -49
	bnez $t0, .main_label9
	b .main_label10
.main_label9:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label10
	b .main_label10
.main_label10:
# %op11 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 2
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 2
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -64
# store i32 6, i32* %op11
	ld.d $t0, $fp, -64
	addi.w $t1, $zero, 6
	st.w $t1, $t0, 0
# %op12 = icmp slt i32 3, 0
	addi.w $t0, $zero, 3
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -65
# br i1 %op12, label %label13, label %label14
	ld.b $t0, $fp, -65
	bnez $t0, .main_label13
	b .main_label14
.main_label13:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label14
	b .main_label14
.main_label14:
# %op15 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 3
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 3
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -80
# store i32 5, i32* %op15
	ld.d $t0, $fp, -80
	addi.w $t1, $zero, 5
	st.w $t1, $t0, 0
# %op16 = icmp slt i32 4, 0
	addi.w $t0, $zero, 4
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -81
# br i1 %op16, label %label17, label %label18
	ld.b $t0, $fp, -81
	bnez $t0, .main_label17
	b .main_label18
.main_label17:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label18
	b .main_label18
.main_label18:
# %op19 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 4
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 4
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -96
# store i32 4, i32* %op19
	ld.d $t0, $fp, -96
	addi.w $t1, $zero, 4
	st.w $t1, $t0, 0
# %op20 = icmp slt i32 0, 0
	addi.w $t0, $zero, 0
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -97
# br i1 %op20, label %label21, label %label22
	ld.b $t0, $fp, -97
	bnez $t0, .main_label21
	b .main_label22
.main_label21:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label22
	b .main_label22
.main_label22:
# %op23 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 0
	la.local $t0, v
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -112
# store i32 6, i32* %op23
	ld.d $t0, $fp, -112
	addi.w $t1, $zero, 6
	st.w $t1, $t0, 0
# %op24 = icmp slt i32 1, 0
	addi.w $t0, $zero, 1
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -113
# br i1 %op24, label %label25, label %label26
	ld.b $t0, $fp, -113
	bnez $t0, .main_label25
	b .main_label26
.main_label25:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label26
	b .main_label26
.main_label26:
# %op27 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 1
	la.local $t0, v
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 1
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -128
# store i32 3, i32* %op27
	ld.d $t0, $fp, -128
	addi.w $t1, $zero, 3
	st.w $t1, $t0, 0
# %op28 = icmp slt i32 2, 0
	addi.w $t0, $zero, 2
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -129
# br i1 %op28, label %label29, label %label30
	ld.b $t0, $fp, -129
	bnez $t0, .main_label29
	b .main_label30
.main_label29:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label30
	b .main_label30
.main_label30:
# %op31 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 2
	la.local $t0, v
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 2
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -144
# store i32 5, i32* %op31
	ld.d $t0, $fp, -144
	addi.w $t1, $zero, 5
	st.w $t1, $t0, 0
# %op32 = icmp slt i32 3, 0
	addi.w $t0, $zero, 3
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -145
# br i1 %op32, label %label33, label %label34
	ld.b $t0, $fp, -145
	bnez $t0, .main_label33
	b .main_label34
.main_label33:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label34
	b .main_label34
.main_label34:
# %op35 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 3
	la.local $t0, v
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 3
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -160
# store i32 4, i32* %op35
	ld.d $t0, $fp, -160
	addi.w $t1, $zero, 4
	st.w $t1, $t0, 0
# %op36 = icmp slt i32 4, 0
	addi.w $t0, $zero, 4
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -161
# br i1 %op36, label %label37, label %label38
	ld.b $t0, $fp, -161
	bnez $t0, .main_label37
	b .main_label38
.main_label37:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label38
	b .main_label38
.main_label38:
# %op39 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 4
	la.local $t0, v
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 4
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -176
# store i32 6, i32* %op39
	ld.d $t0, $fp, -176
	addi.w $t1, $zero, 6
	st.w $t1, $t0, 0
# br label %label40
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -180
	b .main_label40
.main_label40:
# %op41 = phi i32 [ 0, %label38 ], [ %op53, %label50 ]
# %op42 = icmp slt i32 %op41, 66
	ld.w $t0, $fp, -180
	addi.w $t1, $zero, 66
	slt $t2, $t0, $t1
	st.b $t2, $fp, -181
# br i1 %op42, label %label43, label %label45
	ld.b $t0, $fp, -181
	bnez $t0, .main_label43
	b .main_label45
.main_label43:
# %op44 = icmp slt i32 %op41, 0
	ld.w $t0, $fp, -180
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -182
# br i1 %op44, label %label49, label %label50
	ld.b $t0, $fp, -182
	bnez $t0, .main_label49
	b .main_label50
.main_label45:
# %op46 = load i32, i32* @n
	la.local $t0, n
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -188
# %op47 = load i32, i32* @m
	la.local $t0, m
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -192
# %op48 = call i32 @knapsack(i32 %op46, i32 %op47)
	ld.w $a0, $fp, -188
	ld.w $a1, $fp, -192
	bl knapsack
	st.w $a0, $fp, -196
# call void @output(i32 %op48)
	ld.w $a0, $fp, -196
	bl output
# ret i32 0
	addi.w $a0, $zero, 0
	b main_exit
.main_label49:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label50
	b .main_label50
.main_label50:
# %op51 = getelementptr [66 x i32], [66 x i32]* @dp, i32 0, i32 %op41
	la.local $t0, dp
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -180
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -208
# %op52 = sub i32 0, 1
	addi.w $t0, $zero, 0
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -212
# store i32 %op52, i32* %op51
	ld.d $t0, $fp, -208
	ld.w $t1, $fp, -212
	st.w $t1, $t0, 0
# %op53 = add i32 %op41, 1
	ld.w $t0, $fp, -180
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -216
# br label %label40
	ld.w $t0, $fp, -216
	st.w $t0, $fp, -180
	b .main_label40
main_exit:
	addi.d $sp, $sp, 224
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
