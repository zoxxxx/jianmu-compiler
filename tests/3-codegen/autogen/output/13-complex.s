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
	addi.d $sp, $sp, -80
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
.max_label_entry:
# %op2 = alloca i32
	addi.d $t0, $fp, -36
	st.d $t0, $t0, 4
# store i32 %arg0, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -20
	st.w $t1, $t0, 0
# %op3 = alloca i32
	addi.d $t0, $fp, -52
	st.d $t0, $t0, 4
# store i32 %arg1, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t1, $fp, -24
	st.w $t1, $t0, 0
# %op4 = load i32, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -56
# %op5 = load i32, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -60
# %op6 = icmp sgt i32 %op4, %op5
	ld.w $t0, $fp, -56
	ld.w $t1, $fp, -60
	slt $t2, $t1, $t0
	st.b $t2, $fp, -61
# br i1 %op6, label %label7, label %label9
	ld.b $t0, $fp, -61
	bnez $t0, .max_label7
	b .max_label9
.max_label7:
# %op8 = load i32, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -68
# ret i32 %op8
	ld.w $a0, $fp, -68
	b max_exit
.max_label9:
# %op10 = load i32, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -72
# ret i32 %op10
	ld.w $a0, $fp, -72
	b max_exit
.max_label11:
# ret i32 0
	addi.w $a0, $zero, 0
	b max_exit
max_exit:
	addi.d $sp, $sp, 80
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl knapsack
	.type knapsack, @function
knapsack:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -352
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
.knapsack_label_entry:
# %op2 = alloca i32
	addi.d $t0, $fp, -36
	st.d $t0, $t0, 4
# store i32 %arg0, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -20
	st.w $t1, $t0, 0
# %op3 = alloca i32
	addi.d $t0, $fp, -52
	st.d $t0, $t0, 4
# store i32 %arg1, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t1, $fp, -24
	st.w $t1, $t0, 0
# %op4 = alloca i32
	addi.d $t0, $fp, -68
	st.d $t0, $t0, 4
# %op5 = load i32, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -72
# %op6 = icmp sle i32 %op5, 0
	ld.w $t0, $fp, -72
	addi.w $t1, $zero, 0
	slt $t2, $t1, $t0
	xori $t2, $t2, 1
	st.b $t2, $fp, -73
# br i1 %op6, label %label7, label %label8
	ld.b $t0, $fp, -73
	bnez $t0, .knapsack_label7
	b .knapsack_label8
.knapsack_label7:
# ret i32 0
	addi.w $a0, $zero, 0
	b knapsack_exit
.knapsack_label8:
# %op9 = load i32, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -80
# %op10 = icmp eq i32 %op9, 0
	ld.w $t0, $fp, -80
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	slt $t3, $t1, $t0
	or $t2, $t2, $t3
	xori $t2, $t2, 1
	st.b $t2, $fp, -81
# br i1 %op10, label %label11, label %label12
	ld.b $t0, $fp, -81
	bnez $t0, .knapsack_label11
	b .knapsack_label12
.knapsack_label11:
# ret i32 0
	addi.w $a0, $zero, 0
	b knapsack_exit
.knapsack_label12:
# %op13 = load i32, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -88
# %op14 = mul i32 %op13, 11
	ld.w $t0, $fp, -88
	addi.w $t1, $zero, 11
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -92
# %op15 = load i32, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -96
# %op16 = add i32 %op14, %op15
	ld.w $t0, $fp, -92
	ld.w $t1, $fp, -96
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -100
# %op17 = icmp slt i32 %op16, 0
	ld.w $t0, $fp, -100
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -101
# br i1 %op17, label %label18, label %label19
	ld.b $t0, $fp, -101
	bnez $t0, .knapsack_label18
	b .knapsack_label19
.knapsack_label18:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label19
	b .knapsack_label19
.knapsack_label19:
# %op20 = getelementptr [66 x i32], [66 x i32]* @dp, i32 0, i32 %op16
	la.local $t0, dp
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -100
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -112
# %op21 = load i32, i32* %op20
	ld.d $t0, $fp, -112
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -116
# %op22 = icmp sge i32 %op21, 0
	ld.w $t0, $fp, -116
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	xori $t2, $t2, 1
	st.b $t2, $fp, -117
# br i1 %op22, label %label23, label %label29
	ld.b $t0, $fp, -117
	bnez $t0, .knapsack_label23
	b .knapsack_label29
.knapsack_label23:
# %op24 = load i32, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -124
# %op25 = mul i32 %op24, 11
	ld.w $t0, $fp, -124
	addi.w $t1, $zero, 11
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -128
# %op26 = load i32, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -132
# %op27 = add i32 %op25, %op26
	ld.w $t0, $fp, -128
	ld.w $t1, $fp, -132
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -136
# %op28 = icmp slt i32 %op27, 0
	ld.w $t0, $fp, -136
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -137
# br i1 %op28, label %label34, label %label35
	ld.b $t0, $fp, -137
	bnez $t0, .knapsack_label34
	b .knapsack_label35
.knapsack_label29:
# %op30 = load i32, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -144
# %op31 = load i32, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -148
# %op32 = sub i32 %op31, 1
	ld.w $t0, $fp, -148
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -152
# %op33 = icmp slt i32 %op32, 0
	ld.w $t0, $fp, -152
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -153
# br i1 %op33, label %label38, label %label39
	ld.b $t0, $fp, -153
	bnez $t0, .knapsack_label38
	b .knapsack_label39
.knapsack_label34:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label35
	b .knapsack_label35
.knapsack_label35:
# %op36 = getelementptr [66 x i32], [66 x i32]* @dp, i32 0, i32 %op27
	la.local $t0, dp
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -136
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -168
# %op37 = load i32, i32* %op36
	ld.d $t0, $fp, -168
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -172
# ret i32 %op37
	ld.w $a0, $fp, -172
	b knapsack_exit
.knapsack_label38:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label39
	b .knapsack_label39
.knapsack_label39:
# %op40 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 %op32
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -152
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -184
# %op41 = load i32, i32* %op40
	ld.d $t0, $fp, -184
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -188
# %op42 = icmp slt i32 %op30, %op41
	ld.w $t0, $fp, -144
	ld.w $t1, $fp, -188
	slt $t2, $t0, $t1
	st.b $t2, $fp, -189
# br i1 %op42, label %label43, label %label48
	ld.b $t0, $fp, -189
	bnez $t0, .knapsack_label43
	b .knapsack_label48
.knapsack_label43:
# %op44 = load i32, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -196
# %op45 = sub i32 %op44, 1
	ld.w $t0, $fp, -196
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -200
# %op46 = load i32, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -204
# %op47 = call i32 @knapsack(i32 %op45, i32 %op46)
	ld.w $a0, $fp, -200
	ld.w $a1, $fp, -204
	bl knapsack
	st.w $a0, $fp, -208
# store i32 %op47, i32* %op4
	ld.d $t0, $fp, -64
	ld.w $t1, $fp, -208
	st.w $t1, $t0, 0
# br label %label59
	b .knapsack_label59
.knapsack_label48:
# %op49 = load i32, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -212
# %op50 = sub i32 %op49, 1
	ld.w $t0, $fp, -212
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -216
# %op51 = load i32, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -220
# %op52 = call i32 @knapsack(i32 %op50, i32 %op51)
	ld.w $a0, $fp, -216
	ld.w $a1, $fp, -220
	bl knapsack
	st.w $a0, $fp, -224
# %op53 = load i32, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -228
# %op54 = sub i32 %op53, 1
	ld.w $t0, $fp, -228
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -232
# %op55 = load i32, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -236
# %op56 = load i32, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -240
# %op57 = sub i32 %op56, 1
	ld.w $t0, $fp, -240
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -244
# %op58 = icmp slt i32 %op57, 0
	ld.w $t0, $fp, -244
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -245
# br i1 %op58, label %label65, label %label66
	ld.b $t0, $fp, -245
	bnez $t0, .knapsack_label65
	b .knapsack_label66
.knapsack_label59:
# %op60 = load i32, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -252
# %op61 = mul i32 %op60, 11
	ld.w $t0, $fp, -252
	addi.w $t1, $zero, 11
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -256
# %op62 = load i32, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -260
# %op63 = add i32 %op61, %op62
	ld.w $t0, $fp, -256
	ld.w $t1, $fp, -260
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -264
# %op64 = icmp slt i32 %op63, 0
	ld.w $t0, $fp, -264
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -265
# br i1 %op64, label %label80, label %label81
	ld.b $t0, $fp, -265
	bnez $t0, .knapsack_label80
	b .knapsack_label81
.knapsack_label65:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label66
	b .knapsack_label66
.knapsack_label66:
# %op67 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 %op57
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -244
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -280
# %op68 = load i32, i32* %op67
	ld.d $t0, $fp, -280
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -284
# %op69 = sub i32 %op55, %op68
	ld.w $t0, $fp, -236
	ld.w $t1, $fp, -284
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -288
# %op70 = call i32 @knapsack(i32 %op54, i32 %op69)
	ld.w $a0, $fp, -232
	ld.w $a1, $fp, -288
	bl knapsack
	st.w $a0, $fp, -292
# %op71 = load i32, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -296
# %op72 = sub i32 %op71, 1
	ld.w $t0, $fp, -296
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -300
# %op73 = icmp slt i32 %op72, 0
	ld.w $t0, $fp, -300
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -301
# br i1 %op73, label %label74, label %label75
	ld.b $t0, $fp, -301
	bnez $t0, .knapsack_label74
	b .knapsack_label75
.knapsack_label74:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label75
	b .knapsack_label75
.knapsack_label75:
# %op76 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 %op72
	la.local $t0, v
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -300
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -312
# %op77 = load i32, i32* %op76
	ld.d $t0, $fp, -312
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -316
# %op78 = add i32 %op70, %op77
	ld.w $t0, $fp, -292
	ld.w $t1, $fp, -316
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -320
# %op79 = call i32 @max(i32 %op52, i32 %op78)
	ld.w $a0, $fp, -224
	ld.w $a1, $fp, -320
	bl max
	st.w $a0, $fp, -324
# store i32 %op79, i32* %op4
	ld.d $t0, $fp, -64
	ld.w $t1, $fp, -324
	st.w $t1, $t0, 0
# br label %label59
	b .knapsack_label59
.knapsack_label80:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label81
	b .knapsack_label81
.knapsack_label81:
# %op82 = getelementptr [66 x i32], [66 x i32]* @dp, i32 0, i32 %op63
	la.local $t0, dp
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -264
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -336
# %op83 = load i32, i32* %op4
	ld.d $t0, $fp, -64
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -340
# store i32 %op83, i32* %op82
	ld.d $t0, $fp, -336
	ld.w $t1, $fp, -340
	st.w $t1, $t0, 0
# %op84 = load i32, i32* %op4
	ld.d $t0, $fp, -64
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -344
# ret i32 %op84
	ld.w $a0, $fp, -344
	b knapsack_exit
knapsack_exit:
	addi.d $sp, $sp, 352
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -240
.main_label_entry:
# %op0 = alloca i32
	addi.d $t0, $fp, -28
	st.d $t0, $t0, 4
# store i32 0, i32* %op0
	ld.d $t0, $fp, -24
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# store i32 5, i32* @n
	la.local $t0, n
	addi.w $t1, $zero, 5
	st.w $t1, $t0, 0
# store i32 10, i32* @m
	la.local $t0, m
	addi.w $t1, $zero, 10
	st.w $t1, $t0, 0
# %op1 = icmp slt i32 0, 0
	addi.w $t0, $zero, 0
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -29
# br i1 %op1, label %label2, label %label3
	ld.b $t0, $fp, -29
	bnez $t0, .main_label2
	b .main_label3
.main_label2:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label3
	b .main_label3
.main_label3:
# %op4 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 0
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -40
# store i32 2, i32* %op4
	ld.d $t0, $fp, -40
	addi.w $t1, $zero, 2
	st.w $t1, $t0, 0
# %op5 = icmp slt i32 1, 0
	addi.w $t0, $zero, 1
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -41
# br i1 %op5, label %label6, label %label7
	ld.b $t0, $fp, -41
	bnez $t0, .main_label6
	b .main_label7
.main_label6:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label7
	b .main_label7
.main_label7:
# %op8 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 1
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 1
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -56
# store i32 2, i32* %op8
	ld.d $t0, $fp, -56
	addi.w $t1, $zero, 2
	st.w $t1, $t0, 0
# %op9 = icmp slt i32 2, 0
	addi.w $t0, $zero, 2
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -57
# br i1 %op9, label %label10, label %label11
	ld.b $t0, $fp, -57
	bnez $t0, .main_label10
	b .main_label11
.main_label10:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label11
	b .main_label11
.main_label11:
# %op12 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 2
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 2
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -72
# store i32 6, i32* %op12
	ld.d $t0, $fp, -72
	addi.w $t1, $zero, 6
	st.w $t1, $t0, 0
# %op13 = icmp slt i32 3, 0
	addi.w $t0, $zero, 3
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -73
# br i1 %op13, label %label14, label %label15
	ld.b $t0, $fp, -73
	bnez $t0, .main_label14
	b .main_label15
.main_label14:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label15
	b .main_label15
.main_label15:
# %op16 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 3
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 3
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -88
# store i32 5, i32* %op16
	ld.d $t0, $fp, -88
	addi.w $t1, $zero, 5
	st.w $t1, $t0, 0
# %op17 = icmp slt i32 4, 0
	addi.w $t0, $zero, 4
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -89
# br i1 %op17, label %label18, label %label19
	ld.b $t0, $fp, -89
	bnez $t0, .main_label18
	b .main_label19
.main_label18:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label19
	b .main_label19
.main_label19:
# %op20 = getelementptr [5 x i32], [5 x i32]* @w, i32 0, i32 4
	la.local $t0, w
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 4
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -104
# store i32 4, i32* %op20
	ld.d $t0, $fp, -104
	addi.w $t1, $zero, 4
	st.w $t1, $t0, 0
# %op21 = icmp slt i32 0, 0
	addi.w $t0, $zero, 0
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -105
# br i1 %op21, label %label22, label %label23
	ld.b $t0, $fp, -105
	bnez $t0, .main_label22
	b .main_label23
.main_label22:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label23
	b .main_label23
.main_label23:
# %op24 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 0
	la.local $t0, v
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -120
# store i32 6, i32* %op24
	ld.d $t0, $fp, -120
	addi.w $t1, $zero, 6
	st.w $t1, $t0, 0
# %op25 = icmp slt i32 1, 0
	addi.w $t0, $zero, 1
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -121
# br i1 %op25, label %label26, label %label27
	ld.b $t0, $fp, -121
	bnez $t0, .main_label26
	b .main_label27
.main_label26:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label27
	b .main_label27
.main_label27:
# %op28 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 1
	la.local $t0, v
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 1
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -136
# store i32 3, i32* %op28
	ld.d $t0, $fp, -136
	addi.w $t1, $zero, 3
	st.w $t1, $t0, 0
# %op29 = icmp slt i32 2, 0
	addi.w $t0, $zero, 2
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -137
# br i1 %op29, label %label30, label %label31
	ld.b $t0, $fp, -137
	bnez $t0, .main_label30
	b .main_label31
.main_label30:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label31
	b .main_label31
.main_label31:
# %op32 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 2
	la.local $t0, v
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 2
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -152
# store i32 5, i32* %op32
	ld.d $t0, $fp, -152
	addi.w $t1, $zero, 5
	st.w $t1, $t0, 0
# %op33 = icmp slt i32 3, 0
	addi.w $t0, $zero, 3
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -153
# br i1 %op33, label %label34, label %label35
	ld.b $t0, $fp, -153
	bnez $t0, .main_label34
	b .main_label35
.main_label34:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label35
	b .main_label35
.main_label35:
# %op36 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 3
	la.local $t0, v
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 3
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -168
# store i32 4, i32* %op36
	ld.d $t0, $fp, -168
	addi.w $t1, $zero, 4
	st.w $t1, $t0, 0
# %op37 = icmp slt i32 4, 0
	addi.w $t0, $zero, 4
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -169
# br i1 %op37, label %label38, label %label39
	ld.b $t0, $fp, -169
	bnez $t0, .main_label38
	b .main_label39
.main_label38:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label39
	b .main_label39
.main_label39:
# %op40 = getelementptr [5 x i32], [5 x i32]* @v, i32 0, i32 4
	la.local $t0, v
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 4
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -184
# store i32 6, i32* %op40
	ld.d $t0, $fp, -184
	addi.w $t1, $zero, 6
	st.w $t1, $t0, 0
# br label %label41
	b .main_label41
.main_label41:
# %op42 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -188
# %op43 = icmp slt i32 %op42, 66
	ld.w $t0, $fp, -188
	addi.w $t1, $zero, 66
	slt $t2, $t0, $t1
	st.b $t2, $fp, -189
# br i1 %op43, label %label44, label %label47
	ld.b $t0, $fp, -189
	bnez $t0, .main_label44
	b .main_label47
.main_label44:
# %op45 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -196
# %op46 = icmp slt i32 %op45, 0
	ld.w $t0, $fp, -196
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -197
# br i1 %op46, label %label51, label %label52
	ld.b $t0, $fp, -197
	bnez $t0, .main_label51
	b .main_label52
.main_label47:
# %op48 = load i32, i32* @n
	la.local $t0, n
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -204
# %op49 = load i32, i32* @m
	la.local $t0, m
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -208
# %op50 = call i32 @knapsack(i32 %op48, i32 %op49)
	ld.w $a0, $fp, -204
	ld.w $a1, $fp, -208
	bl knapsack
	st.w $a0, $fp, -212
# call void @output(i32 %op50)
	ld.w $a0, $fp, -212
	bl output
# ret i32 0
	addi.w $a0, $zero, 0
	b main_exit
.main_label51:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label52
	b .main_label52
.main_label52:
# %op53 = getelementptr [66 x i32], [66 x i32]* @dp, i32 0, i32 %op45
	la.local $t0, dp
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -196
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -224
# %op54 = sub i32 0, 1
	addi.w $t0, $zero, 0
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -228
# store i32 %op54, i32* %op53
	ld.d $t0, $fp, -224
	ld.w $t1, $fp, -228
	st.w $t1, $t0, 0
# %op55 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -232
# %op56 = add i32 %op55, 1
	ld.w $t0, $fp, -232
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -236
# store i32 %op56, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t1, $fp, -236
	st.w $t1, $t0, 0
# br label %label41
	b .main_label41
main_exit:
	addi.d $sp, $sp, 240
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
