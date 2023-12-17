# Global variables
	.text
	.section .bss, "aw", @nobits
	.globl matrix
	.type matrix, @object
	.size matrix, 80000000
matrix:
	.space 80000000
	.globl ad
	.type ad, @object
	.size ad, 400000
ad:
	.space 400000
	.globl len
	.type len, @object
	.size len, 4
len:
	.space 4
	.text
	.globl readarray
	.type readarray, @function
readarray:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -48
.readarray_label_entry:
# br label %label0
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -20
	b .readarray_label0
.readarray_label0:
# %op1 = phi i32 [ 0, %label_entry ], [ %op11, %label8 ]
# %op2 = load i32, i32* @len
	la.local $t0, len
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -24
# %op3 = icmp slt i32 %op1, %op2
	ld.w $t0, $fp, -20
	ld.w $t1, $fp, -24
	slt $t2, $t0, $t1
	st.b $t2, $fp, -25
# br i1 %op3, label %label4, label %label6
	ld.b $t0, $fp, -25
	bnez $t0, .readarray_label4
	b .readarray_label6
.readarray_label4:
# %op5 = icmp slt i32 %op1, 0
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -26
# br i1 %op5, label %label7, label %label8
	ld.b $t0, $fp, -26
	bnez $t0, .readarray_label7
	b .readarray_label8
.readarray_label6:
# ret void
	addi.d $a0, $zero, 0
	b readarray_exit
.readarray_label7:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label8
	b .readarray_label8
.readarray_label8:
# %op9 = getelementptr [100000 x i32], [100000 x i32]* @ad, i32 0, i32 %op1
	la.local $t0, ad
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -20
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -40
# %op10 = call i32 @input()
	bl input
	st.w $a0, $fp, -44
# store i32 %op10, i32* %op9
	ld.d $t0, $fp, -40
	ld.w $t1, $fp, -44
	st.w $t1, $t0, 0
# %op11 = add i32 %op1, 1
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -48
# br label %label0
	ld.w $t0, $fp, -48
	st.w $t0, $fp, -20
	b .readarray_label0
readarray_exit:
	addi.d $sp, $sp, 48
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl transpose
	.type transpose, @function
transpose:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -176
	st.w $a0, $fp, -20
	st.d $a1, $fp, -32
	st.w $a2, $fp, -36
.transpose_label_entry:
# %op3 = sdiv i32 %arg0, %arg2
	ld.w $t0, $fp, -20
	ld.w $t1, $fp, -36
	div.w $t2, $t0, $t1
	st.w $t2, $fp, -40
# br label %label4
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -44
	b .transpose_label4
.transpose_label4:
# %op5 = phi i32 [ 0, %label_entry ], [ %op16, %label15 ]
# %op6 = icmp slt i32 %op5, %op3
	ld.w $t0, $fp, -44
	ld.w $t1, $fp, -40
	slt $t2, $t0, $t1
	st.b $t2, $fp, -45
# br i1 %op6, label %label7, label %label8
	ld.b $t0, $fp, -45
	bnez $t0, .transpose_label7
	b .transpose_label8
.transpose_label7:
# br label %label10
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -56
	b .transpose_label10
.transpose_label8:
# %op9 = sub i32 0, 1
	addi.w $t0, $zero, 0
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -52
# ret i32 %op9
	ld.w $a0, $fp, -52
	b transpose_exit
.transpose_label10:
# %op11 = phi i32 [ 0, %label7 ], [ %op24, %label23 ]
# %op12 = icmp slt i32 %op11, %arg2
	ld.w $t0, $fp, -56
	ld.w $t1, $fp, -36
	slt $t2, $t0, $t1
	st.b $t2, $fp, -57
# br i1 %op12, label %label13, label %label15
	ld.b $t0, $fp, -57
	bnez $t0, .transpose_label13
	b .transpose_label15
.transpose_label13:
# %op14 = icmp slt i32 %op5, %op11
	ld.w $t0, $fp, -44
	ld.w $t1, $fp, -56
	slt $t2, $t0, $t1
	st.b $t2, $fp, -58
# br i1 %op14, label %label17, label %label19
	ld.b $t0, $fp, -58
	bnez $t0, .transpose_label17
	b .transpose_label19
.transpose_label15:
# %op16 = add i32 %op5, 1
	ld.w $t0, $fp, -44
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -64
# br label %label4
	ld.w $t0, $fp, -64
	st.w $t0, $fp, -44
	b .transpose_label4
.transpose_label17:
# %op18 = add i32 %op11, 1
	ld.w $t0, $fp, -56
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -68
# br label %label23
	ld.w $t0, $fp, -68
	st.w $t0, $fp, -84
	b .transpose_label23
.transpose_label19:
# %op20 = mul i32 %op5, %arg2
	ld.w $t0, $fp, -44
	ld.w $t1, $fp, -36
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -72
# %op21 = add i32 %op20, %op11
	ld.w $t0, $fp, -72
	ld.w $t1, $fp, -56
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -76
# %op22 = icmp slt i32 %op21, 0
	ld.w $t0, $fp, -76
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -77
# br i1 %op22, label %label25, label %label26
	ld.b $t0, $fp, -77
	bnez $t0, .transpose_label25
	b .transpose_label26
.transpose_label23:
# %op24 = phi i32 [ %op18, %label17 ], [ %op48, %label46 ]
# br label %label10
	ld.w $t0, $fp, -84
	st.w $t0, $fp, -56
	b .transpose_label10
.transpose_label25:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label26
	b .transpose_label26
.transpose_label26:
# %op27 = getelementptr i32, i32* %arg1, i32 %op21
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -76
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -96
# %op28 = load i32, i32* %op27
	ld.d $t0, $fp, -96
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -100
# %op29 = mul i32 %op11, %op3
	ld.w $t0, $fp, -56
	ld.w $t1, $fp, -40
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -104
# %op30 = add i32 %op29, %op5
	ld.w $t0, $fp, -104
	ld.w $t1, $fp, -44
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -108
# %op31 = icmp slt i32 %op30, 0
	ld.w $t0, $fp, -108
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -109
# br i1 %op31, label %label32, label %label33
	ld.b $t0, $fp, -109
	bnez $t0, .transpose_label32
	b .transpose_label33
.transpose_label32:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label33
	b .transpose_label33
.transpose_label33:
# %op34 = getelementptr i32, i32* %arg1, i32 %op30
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -108
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -120
# %op35 = mul i32 %op5, %arg2
	ld.w $t0, $fp, -44
	ld.w $t1, $fp, -36
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -124
# %op36 = add i32 %op35, %op11
	ld.w $t0, $fp, -124
	ld.w $t1, $fp, -56
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -128
# %op37 = icmp slt i32 %op36, 0
	ld.w $t0, $fp, -128
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -129
# br i1 %op37, label %label38, label %label39
	ld.b $t0, $fp, -129
	bnez $t0, .transpose_label38
	b .transpose_label39
.transpose_label38:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label39
	b .transpose_label39
.transpose_label39:
# %op40 = getelementptr i32, i32* %arg1, i32 %op36
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -128
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -144
# %op41 = load i32, i32* %op40
	ld.d $t0, $fp, -144
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -148
# store i32 %op41, i32* %op34
	ld.d $t0, $fp, -120
	ld.w $t1, $fp, -148
	st.w $t1, $t0, 0
# %op42 = mul i32 %op5, %arg2
	ld.w $t0, $fp, -44
	ld.w $t1, $fp, -36
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -152
# %op43 = add i32 %op42, %op11
	ld.w $t0, $fp, -152
	ld.w $t1, $fp, -56
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -156
# %op44 = icmp slt i32 %op43, 0
	ld.w $t0, $fp, -156
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -157
# br i1 %op44, label %label45, label %label46
	ld.b $t0, $fp, -157
	bnez $t0, .transpose_label45
	b .transpose_label46
.transpose_label45:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label46
	b .transpose_label46
.transpose_label46:
# %op47 = getelementptr i32, i32* %arg1, i32 %op43
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -156
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -168
# store i32 %op28, i32* %op47
	ld.d $t0, $fp, -168
	ld.w $t1, $fp, -100
	st.w $t1, $t0, 0
# %op48 = add i32 %op11, 1
	ld.w $t0, $fp, -56
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -172
# br label %label23
	ld.w $t0, $fp, -172
	st.w $t0, $fp, -84
	b .transpose_label23
transpose_exit:
	addi.d $sp, $sp, 176
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -160
.main_label_entry:
# %op0 = call i32 @input()
	bl input
	st.w $a0, $fp, -20
# %op1 = call i32 @input()
	bl input
	st.w $a0, $fp, -24
# store i32 %op1, i32* @len
	la.local $t0, len
	ld.w $t1, $fp, -24
	st.w $t1, $t0, 0
# call void @readarray()
	bl readarray
# br label %label2
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -28
	b .main_label2
.main_label2:
# %op3 = phi i32 [ 0, %label_entry ], [ %op11, %label9 ]
# %op4 = icmp slt i32 %op3, %op0
	ld.w $t0, $fp, -28
	ld.w $t1, $fp, -20
	slt $t2, $t0, $t1
	st.b $t2, $fp, -29
# br i1 %op4, label %label5, label %label7
	ld.b $t0, $fp, -29
	bnez $t0, .main_label5
	b .main_label7
.main_label5:
# %op6 = icmp slt i32 %op3, 0
	ld.w $t0, $fp, -28
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -30
# br i1 %op6, label %label8, label %label9
	ld.b $t0, $fp, -30
	bnez $t0, .main_label8
	b .main_label9
.main_label7:
# br label %label12
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -48
	b .main_label12
.main_label8:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label9
	b .main_label9
.main_label9:
# %op10 = getelementptr [20000000 x i32], [20000000 x i32]* @matrix, i32 0, i32 %op3
	la.local $t0, matrix
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -28
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -40
# store i32 %op3, i32* %op10
	ld.d $t0, $fp, -40
	ld.w $t1, $fp, -28
	st.w $t1, $t0, 0
# %op11 = add i32 %op3, 1
	ld.w $t0, $fp, -28
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -44
# br label %label2
	ld.w $t0, $fp, -44
	st.w $t0, $fp, -28
	b .main_label2
.main_label12:
# %op13 = phi i32 [ 0, %label7 ], [ %op25, %label21 ]
# %op14 = load i32, i32* @len
	la.local $t0, len
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -52
# %op15 = icmp slt i32 %op13, %op14
	ld.w $t0, $fp, -48
	ld.w $t1, $fp, -52
	slt $t2, $t0, $t1
	st.b $t2, $fp, -53
# br i1 %op15, label %label16, label %label19
	ld.b $t0, $fp, -53
	bnez $t0, .main_label16
	b .main_label19
.main_label16:
# %op17 = getelementptr [20000000 x i32], [20000000 x i32]* @matrix, i32 0, i32 0
	la.local $t0, matrix
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -64
# %op18 = icmp slt i32 %op13, 0
	ld.w $t0, $fp, -48
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -65
# br i1 %op18, label %label20, label %label21
	ld.b $t0, $fp, -65
	bnez $t0, .main_label20
	b .main_label21
.main_label19:
# br label %label26
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -96
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -100
	b .main_label26
.main_label20:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label21
	b .main_label21
.main_label21:
# %op22 = getelementptr [100000 x i32], [100000 x i32]* @ad, i32 0, i32 %op13
	la.local $t0, ad
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -48
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -80
# %op23 = load i32, i32* %op22
	ld.d $t0, $fp, -80
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -84
# %op24 = call i32 @transpose(i32 %op0, i32* %op17, i32 %op23)
	ld.w $a0, $fp, -20
	ld.d $a1, $fp, -64
	ld.w $a2, $fp, -84
	bl transpose
	st.w $a0, $fp, -88
# %op25 = add i32 %op13, 1
	ld.w $t0, $fp, -48
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -92
# br label %label12
	ld.w $t0, $fp, -92
	st.w $t0, $fp, -48
	b .main_label12
.main_label26:
# %op27 = phi i32 [ 0, %label19 ], [ %op41, %label37 ]
# %op28 = phi i32 [ 0, %label19 ], [ %op42, %label37 ]
# %op29 = load i32, i32* @len
	la.local $t0, len
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -104
# %op30 = icmp slt i32 %op28, %op29
	ld.w $t0, $fp, -100
	ld.w $t1, $fp, -104
	slt $t2, $t0, $t1
	st.b $t2, $fp, -105
# br i1 %op30, label %label31, label %label34
	ld.b $t0, $fp, -105
	bnez $t0, .main_label31
	b .main_label34
.main_label31:
# %op32 = mul i32 %op28, %op28
	ld.w $t0, $fp, -100
	ld.w $t1, $fp, -100
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -112
# %op33 = icmp slt i32 %op28, 0
	ld.w $t0, $fp, -100
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -113
# br i1 %op33, label %label36, label %label37
	ld.b $t0, $fp, -113
	bnez $t0, .main_label36
	b .main_label37
.main_label34:
# %op35 = icmp slt i32 %op27, 0
	ld.w $t0, $fp, -96
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -114
# br i1 %op35, label %label43, label %label45
	ld.w $t0, $fp, -96
	st.w $t0, $fp, -152
	ld.b $t0, $fp, -114
	bnez $t0, .main_label43
	b .main_label45
.main_label36:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label37
	b .main_label37
.main_label37:
# %op38 = getelementptr [20000000 x i32], [20000000 x i32]* @matrix, i32 0, i32 %op28
	la.local $t0, matrix
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -100
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -128
# %op39 = load i32, i32* %op38
	ld.d $t0, $fp, -128
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -132
# %op40 = mul i32 %op32, %op39
	ld.w $t0, $fp, -112
	ld.w $t1, $fp, -132
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -136
# %op41 = add i32 %op27, %op40
	ld.w $t0, $fp, -96
	ld.w $t1, $fp, -136
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -140
# %op42 = add i32 %op28, 1
	ld.w $t0, $fp, -100
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -144
# br label %label26
	ld.w $t0, $fp, -140
	st.w $t0, $fp, -96
	ld.w $t0, $fp, -144
	st.w $t0, $fp, -100
	b .main_label26
.main_label43:
# %op44 = sub i32 0, %op27
	addi.w $t0, $zero, 0
	ld.w $t1, $fp, -96
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -148
# br label %label45
	ld.w $t0, $fp, -148
	st.w $t0, $fp, -152
	b .main_label45
.main_label45:
# %op46 = phi i32 [ %op27, %label34 ], [ %op44, %label43 ]
# call void @output(i32 %op46)
	ld.w $a0, $fp, -152
	bl output
# ret i32 0
	addi.w $a0, $zero, 0
	b main_exit
main_exit:
	addi.d $sp, $sp, 160
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
