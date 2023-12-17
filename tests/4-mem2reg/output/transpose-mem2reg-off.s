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
	addi.d $sp, $sp, -80
.readarray_label_entry:
# %op0 = alloca i32
	addi.d $t0, $fp, -28
	st.d $t0, $t0, 4
# store i32 0, i32* %op0
	ld.d $t0, $fp, -24
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# br label %label1
	b .readarray_label1
.readarray_label1:
# %op2 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -32
# %op3 = load i32, i32* @len
	la.local $t0, len
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -36
# %op4 = icmp slt i32 %op2, %op3
	ld.w $t0, $fp, -32
	ld.w $t1, $fp, -36
	slt $t2, $t0, $t1
	st.b $t2, $fp, -37
# br i1 %op4, label %label5, label %label8
	ld.b $t0, $fp, -37
	bnez $t0, .readarray_label5
	b .readarray_label8
.readarray_label5:
# %op6 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -44
# %op7 = icmp slt i32 %op6, 0
	ld.w $t0, $fp, -44
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -45
# br i1 %op7, label %label9, label %label10
	ld.b $t0, $fp, -45
	bnez $t0, .readarray_label9
	b .readarray_label10
.readarray_label8:
# ret void
	addi.d $a0, $zero, 0
	b readarray_exit
.readarray_label9:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label10
	b .readarray_label10
.readarray_label10:
# %op11 = getelementptr [100000 x i32], [100000 x i32]* @ad, i32 0, i32 %op6
	la.local $t0, ad
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -44
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -56
# %op12 = call i32 @input()
	bl input
	st.w $a0, $fp, -60
# store i32 %op12, i32* %op11
	ld.d $t0, $fp, -56
	ld.w $t1, $fp, -60
	st.w $t1, $t0, 0
# %op13 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -64
# %op14 = add i32 %op13, 1
	ld.w $t0, $fp, -64
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -68
# store i32 %op14, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t1, $fp, -68
	st.w $t1, $t0, 0
# br label %label1
	b .readarray_label1
readarray_exit:
	addi.d $sp, $sp, 80
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl transpose
	.type transpose, @function
transpose:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -416
	st.w $a0, $fp, -20
	st.d $a1, $fp, -32
	st.w $a2, $fp, -36
.transpose_label_entry:
# %op3 = alloca i32
	addi.d $t0, $fp, -52
	st.d $t0, $t0, 4
# store i32 %arg0, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t1, $fp, -20
	st.w $t1, $t0, 0
# %op4 = alloca i32*
	addi.d $t0, $fp, -72
	st.d $t0, $t0, 8
# store i32* %arg1, i32** %op4
	ld.d $t0, $fp, -64
	ld.d $t1, $fp, -32
	st.d $t1, $t0, 0
# %op5 = alloca i32
	addi.d $t0, $fp, -84
	st.d $t0, $t0, 4
# store i32 %arg2, i32* %op5
	ld.d $t0, $fp, -80
	ld.w $t1, $fp, -36
	st.w $t1, $t0, 0
# %op6 = alloca i32
	addi.d $t0, $fp, -100
	st.d $t0, $t0, 4
# %op7 = alloca i32
	addi.d $t0, $fp, -116
	st.d $t0, $t0, 4
# %op8 = alloca i32
	addi.d $t0, $fp, -132
	st.d $t0, $t0, 4
# %op9 = alloca i32
	addi.d $t0, $fp, -148
	st.d $t0, $t0, 4
# %op10 = load i32, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -152
# %op11 = load i32, i32* %op5
	ld.d $t0, $fp, -80
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -156
# %op12 = sdiv i32 %op10, %op11
	ld.w $t0, $fp, -152
	ld.w $t1, $fp, -156
	div.w $t2, $t0, $t1
	st.w $t2, $fp, -160
# store i32 %op12, i32* %op6
	ld.d $t0, $fp, -96
	ld.w $t1, $fp, -160
	st.w $t1, $t0, 0
# store i32 0, i32* %op7
	ld.d $t0, $fp, -112
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# store i32 0, i32* %op8
	ld.d $t0, $fp, -128
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# br label %label13
	b .transpose_label13
.transpose_label13:
# %op14 = load i32, i32* %op7
	ld.d $t0, $fp, -112
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -164
# %op15 = load i32, i32* %op6
	ld.d $t0, $fp, -96
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -168
# %op16 = icmp slt i32 %op14, %op15
	ld.w $t0, $fp, -164
	ld.w $t1, $fp, -168
	slt $t2, $t0, $t1
	st.b $t2, $fp, -169
# br i1 %op16, label %label17, label %label18
	ld.b $t0, $fp, -169
	bnez $t0, .transpose_label17
	b .transpose_label18
.transpose_label17:
# store i32 0, i32* %op8
	ld.d $t0, $fp, -128
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# br label %label20
	b .transpose_label20
.transpose_label18:
# %op19 = sub i32 0, 1
	addi.w $t0, $zero, 0
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -176
# ret i32 %op19
	ld.w $a0, $fp, -176
	b transpose_exit
.transpose_label20:
# %op21 = load i32, i32* %op8
	ld.d $t0, $fp, -128
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -180
# %op22 = load i32, i32* %op5
	ld.d $t0, $fp, -80
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -184
# %op23 = icmp slt i32 %op21, %op22
	ld.w $t0, $fp, -180
	ld.w $t1, $fp, -184
	slt $t2, $t0, $t1
	st.b $t2, $fp, -185
# br i1 %op23, label %label24, label %label28
	ld.b $t0, $fp, -185
	bnez $t0, .transpose_label24
	b .transpose_label28
.transpose_label24:
# %op25 = load i32, i32* %op7
	ld.d $t0, $fp, -112
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -192
# %op26 = load i32, i32* %op8
	ld.d $t0, $fp, -128
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -196
# %op27 = icmp slt i32 %op25, %op26
	ld.w $t0, $fp, -192
	ld.w $t1, $fp, -196
	slt $t2, $t0, $t1
	st.b $t2, $fp, -197
# br i1 %op27, label %label31, label %label34
	ld.b $t0, $fp, -197
	bnez $t0, .transpose_label31
	b .transpose_label34
.transpose_label28:
# %op29 = load i32, i32* %op7
	ld.d $t0, $fp, -112
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -204
# %op30 = add i32 %op29, 1
	ld.w $t0, $fp, -204
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -208
# store i32 %op30, i32* %op7
	ld.d $t0, $fp, -112
	ld.w $t1, $fp, -208
	st.w $t1, $t0, 0
# br label %label13
	b .transpose_label13
.transpose_label31:
# %op32 = load i32, i32* %op8
	ld.d $t0, $fp, -128
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -212
# %op33 = add i32 %op32, 1
	ld.w $t0, $fp, -212
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -216
# store i32 %op33, i32* %op8
	ld.d $t0, $fp, -128
	ld.w $t1, $fp, -216
	st.w $t1, $t0, 0
# br label %label41
	b .transpose_label41
.transpose_label34:
# %op35 = load i32, i32* %op7
	ld.d $t0, $fp, -112
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -220
# %op36 = load i32, i32* %op5
	ld.d $t0, $fp, -80
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -224
# %op37 = mul i32 %op35, %op36
	ld.w $t0, $fp, -220
	ld.w $t1, $fp, -224
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -228
# %op38 = load i32, i32* %op8
	ld.d $t0, $fp, -128
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -232
# %op39 = add i32 %op37, %op38
	ld.w $t0, $fp, -228
	ld.w $t1, $fp, -232
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -236
# %op40 = icmp slt i32 %op39, 0
	ld.w $t0, $fp, -236
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -237
# br i1 %op40, label %label42, label %label43
	ld.b $t0, $fp, -237
	bnez $t0, .transpose_label42
	b .transpose_label43
.transpose_label41:
# br label %label20
	b .transpose_label20
.transpose_label42:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label43
	b .transpose_label43
.transpose_label43:
# %op44 = load i32*, i32** %op4
	ld.d $t0, $fp, -64
	ld.d $t0, $t0, 0
	st.d $t0, $fp, -248
# %op45 = getelementptr i32, i32* %op44, i32 %op39
	ld.d $t0, $fp, -248
	ld.w $t1, $fp, -236
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -256
# %op46 = load i32, i32* %op45
	ld.d $t0, $fp, -256
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -260
# store i32 %op46, i32* %op9
	ld.d $t0, $fp, -144
	ld.w $t1, $fp, -260
	st.w $t1, $t0, 0
# %op47 = load i32, i32* %op8
	ld.d $t0, $fp, -128
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -264
# %op48 = load i32, i32* %op6
	ld.d $t0, $fp, -96
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -268
# %op49 = mul i32 %op47, %op48
	ld.w $t0, $fp, -264
	ld.w $t1, $fp, -268
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -272
# %op50 = load i32, i32* %op7
	ld.d $t0, $fp, -112
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -276
# %op51 = add i32 %op49, %op50
	ld.w $t0, $fp, -272
	ld.w $t1, $fp, -276
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -280
# %op52 = icmp slt i32 %op51, 0
	ld.w $t0, $fp, -280
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -281
# br i1 %op52, label %label53, label %label54
	ld.b $t0, $fp, -281
	bnez $t0, .transpose_label53
	b .transpose_label54
.transpose_label53:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label54
	b .transpose_label54
.transpose_label54:
# %op55 = load i32*, i32** %op4
	ld.d $t0, $fp, -64
	ld.d $t0, $t0, 0
	st.d $t0, $fp, -296
# %op56 = getelementptr i32, i32* %op55, i32 %op51
	ld.d $t0, $fp, -296
	ld.w $t1, $fp, -280
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -304
# %op57 = load i32, i32* %op7
	ld.d $t0, $fp, -112
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -308
# %op58 = load i32, i32* %op5
	ld.d $t0, $fp, -80
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -312
# %op59 = mul i32 %op57, %op58
	ld.w $t0, $fp, -308
	ld.w $t1, $fp, -312
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -316
# %op60 = load i32, i32* %op8
	ld.d $t0, $fp, -128
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -320
# %op61 = add i32 %op59, %op60
	ld.w $t0, $fp, -316
	ld.w $t1, $fp, -320
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -324
# %op62 = icmp slt i32 %op61, 0
	ld.w $t0, $fp, -324
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -325
# br i1 %op62, label %label63, label %label64
	ld.b $t0, $fp, -325
	bnez $t0, .transpose_label63
	b .transpose_label64
.transpose_label63:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label64
	b .transpose_label64
.transpose_label64:
# %op65 = load i32*, i32** %op4
	ld.d $t0, $fp, -64
	ld.d $t0, $t0, 0
	st.d $t0, $fp, -336
# %op66 = getelementptr i32, i32* %op65, i32 %op61
	ld.d $t0, $fp, -336
	ld.w $t1, $fp, -324
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -344
# %op67 = load i32, i32* %op66
	ld.d $t0, $fp, -344
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -348
# store i32 %op67, i32* %op56
	ld.d $t0, $fp, -304
	ld.w $t1, $fp, -348
	st.w $t1, $t0, 0
# %op68 = load i32, i32* %op7
	ld.d $t0, $fp, -112
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -352
# %op69 = load i32, i32* %op5
	ld.d $t0, $fp, -80
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -356
# %op70 = mul i32 %op68, %op69
	ld.w $t0, $fp, -352
	ld.w $t1, $fp, -356
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -360
# %op71 = load i32, i32* %op8
	ld.d $t0, $fp, -128
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -364
# %op72 = add i32 %op70, %op71
	ld.w $t0, $fp, -360
	ld.w $t1, $fp, -364
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -368
# %op73 = icmp slt i32 %op72, 0
	ld.w $t0, $fp, -368
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -369
# br i1 %op73, label %label74, label %label75
	ld.b $t0, $fp, -369
	bnez $t0, .transpose_label74
	b .transpose_label75
.transpose_label74:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label75
	b .transpose_label75
.transpose_label75:
# %op76 = load i32*, i32** %op4
	ld.d $t0, $fp, -64
	ld.d $t0, $t0, 0
	st.d $t0, $fp, -384
# %op77 = getelementptr i32, i32* %op76, i32 %op72
	ld.d $t0, $fp, -384
	ld.w $t1, $fp, -368
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -392
# %op78 = load i32, i32* %op9
	ld.d $t0, $fp, -144
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -396
# store i32 %op78, i32* %op77
	ld.d $t0, $fp, -392
	ld.w $t1, $fp, -396
	st.w $t1, $t0, 0
# %op79 = load i32, i32* %op8
	ld.d $t0, $fp, -128
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -400
# %op80 = add i32 %op79, 1
	ld.w $t0, $fp, -400
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -404
# store i32 %op80, i32* %op8
	ld.d $t0, $fp, -128
	ld.w $t1, $fp, -404
	st.w $t1, $t0, 0
# br label %label41
	b .transpose_label41
transpose_exit:
	addi.d $sp, $sp, 416
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -256
.main_label_entry:
# %op0 = alloca i32
	addi.d $t0, $fp, -28
	st.d $t0, $t0, 4
# %op1 = alloca i32
	addi.d $t0, $fp, -44
	st.d $t0, $t0, 4
# %op2 = alloca i32
	addi.d $t0, $fp, -60
	st.d $t0, $t0, 4
# %op3 = call i32 @input()
	bl input
	st.w $a0, $fp, -64
# store i32 %op3, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t1, $fp, -64
	st.w $t1, $t0, 0
# %op4 = call i32 @input()
	bl input
	st.w $a0, $fp, -68
# store i32 %op4, i32* @len
	la.local $t0, len
	ld.w $t1, $fp, -68
	st.w $t1, $t0, 0
# call void @readarray()
	bl readarray
# store i32 0, i32* %op1
	ld.d $t0, $fp, -40
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# br label %label5
	b .main_label5
.main_label5:
# %op6 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -72
# %op7 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -76
# %op8 = icmp slt i32 %op6, %op7
	ld.w $t0, $fp, -72
	ld.w $t1, $fp, -76
	slt $t2, $t0, $t1
	st.b $t2, $fp, -77
# br i1 %op8, label %label9, label %label12
	ld.b $t0, $fp, -77
	bnez $t0, .main_label9
	b .main_label12
.main_label9:
# %op10 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -84
# %op11 = icmp slt i32 %op10, 0
	ld.w $t0, $fp, -84
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -85
# br i1 %op11, label %label13, label %label14
	ld.b $t0, $fp, -85
	bnez $t0, .main_label13
	b .main_label14
.main_label12:
# store i32 0, i32* %op1
	ld.d $t0, $fp, -40
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# br label %label19
	b .main_label19
.main_label13:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label14
	b .main_label14
.main_label14:
# %op15 = getelementptr [20000000 x i32], [20000000 x i32]* @matrix, i32 0, i32 %op10
	la.local $t0, matrix
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -84
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -96
# %op16 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -100
# store i32 %op16, i32* %op15
	ld.d $t0, $fp, -96
	ld.w $t1, $fp, -100
	st.w $t1, $t0, 0
# %op17 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -104
# %op18 = add i32 %op17, 1
	ld.w $t0, $fp, -104
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -108
# store i32 %op18, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t1, $fp, -108
	st.w $t1, $t0, 0
# br label %label5
	b .main_label5
.main_label19:
# %op20 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -112
# %op21 = load i32, i32* @len
	la.local $t0, len
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -116
# %op22 = icmp slt i32 %op20, %op21
	ld.w $t0, $fp, -112
	ld.w $t1, $fp, -116
	slt $t2, $t0, $t1
	st.b $t2, $fp, -117
# br i1 %op22, label %label23, label %label28
	ld.b $t0, $fp, -117
	bnez $t0, .main_label23
	b .main_label28
.main_label23:
# %op24 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -124
# %op25 = getelementptr [20000000 x i32], [20000000 x i32]* @matrix, i32 0, i32 0
	la.local $t0, matrix
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -136
# %op26 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -140
# %op27 = icmp slt i32 %op26, 0
	ld.w $t0, $fp, -140
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -141
# br i1 %op27, label %label29, label %label30
	ld.b $t0, $fp, -141
	bnez $t0, .main_label29
	b .main_label30
.main_label28:
# store i32 0, i32* %op2
	ld.d $t0, $fp, -56
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# store i32 0, i32* %op1
	ld.d $t0, $fp, -40
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# br label %label36
	b .main_label36
.main_label29:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label30
	b .main_label30
.main_label30:
# %op31 = getelementptr [100000 x i32], [100000 x i32]* @ad, i32 0, i32 %op26
	la.local $t0, ad
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -140
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -152
# %op32 = load i32, i32* %op31
	ld.d $t0, $fp, -152
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -156
# %op33 = call i32 @transpose(i32 %op24, i32* %op25, i32 %op32)
	ld.w $a0, $fp, -124
	ld.d $a1, $fp, -136
	ld.w $a2, $fp, -156
	bl transpose
	st.w $a0, $fp, -160
# %op34 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -164
# %op35 = add i32 %op34, 1
	ld.w $t0, $fp, -164
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -168
# store i32 %op35, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t1, $fp, -168
	st.w $t1, $t0, 0
# br label %label19
	b .main_label19
.main_label36:
# %op37 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -172
# %op38 = load i32, i32* @len
	la.local $t0, len
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -176
# %op39 = icmp slt i32 %op37, %op38
	ld.w $t0, $fp, -172
	ld.w $t1, $fp, -176
	slt $t2, $t0, $t1
	st.b $t2, $fp, -177
# br i1 %op39, label %label40, label %label47
	ld.b $t0, $fp, -177
	bnez $t0, .main_label40
	b .main_label47
.main_label40:
# %op41 = load i32, i32* %op2
	ld.d $t0, $fp, -56
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -184
# %op42 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -188
# %op43 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -192
# %op44 = mul i32 %op42, %op43
	ld.w $t0, $fp, -188
	ld.w $t1, $fp, -192
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -196
# %op45 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -200
# %op46 = icmp slt i32 %op45, 0
	ld.w $t0, $fp, -200
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -201
# br i1 %op46, label %label50, label %label51
	ld.b $t0, $fp, -201
	bnez $t0, .main_label50
	b .main_label51
.main_label47:
# %op48 = load i32, i32* %op2
	ld.d $t0, $fp, -56
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -208
# %op49 = icmp slt i32 %op48, 0
	ld.w $t0, $fp, -208
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -209
# br i1 %op49, label %label58, label %label61
	ld.b $t0, $fp, -209
	bnez $t0, .main_label58
	b .main_label61
.main_label50:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label51
	b .main_label51
.main_label51:
# %op52 = getelementptr [20000000 x i32], [20000000 x i32]* @matrix, i32 0, i32 %op45
	la.local $t0, matrix
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -200
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -224
# %op53 = load i32, i32* %op52
	ld.d $t0, $fp, -224
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -228
# %op54 = mul i32 %op44, %op53
	ld.w $t0, $fp, -196
	ld.w $t1, $fp, -228
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -232
# %op55 = add i32 %op41, %op54
	ld.w $t0, $fp, -184
	ld.w $t1, $fp, -232
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -236
# store i32 %op55, i32* %op2
	ld.d $t0, $fp, -56
	ld.w $t1, $fp, -236
	st.w $t1, $t0, 0
# %op56 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -240
# %op57 = add i32 %op56, 1
	ld.w $t0, $fp, -240
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -244
# store i32 %op57, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t1, $fp, -244
	st.w $t1, $t0, 0
# br label %label36
	b .main_label36
.main_label58:
# %op59 = load i32, i32* %op2
	ld.d $t0, $fp, -56
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -248
# %op60 = sub i32 0, %op59
	addi.w $t0, $zero, 0
	ld.w $t1, $fp, -248
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -252
# store i32 %op60, i32* %op2
	ld.d $t0, $fp, -56
	ld.w $t1, $fp, -252
	st.w $t1, $t0, 0
# br label %label61
	b .main_label61
.main_label61:
# %op62 = load i32, i32* %op2
	ld.d $t0, $fp, -56
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -256
# call void @output(i32 %op62)
	ld.w $a0, $fp, -256
	bl output
# ret i32 0
	addi.w $a0, $zero, 0
	b main_exit
main_exit:
	addi.d $sp, $sp, 256
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
