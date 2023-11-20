	.text
	.globl store
	.type store, @function
store:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -112
	st.d $a0, $fp, -24
	st.w $a1, $fp, -28
	st.w $a2, $fp, -32
.store_label_entry:
# %op3 = alloca i32*
	addi.d $t0, $fp, -48
	st.d $t0, $t0, 8
# store i32* %arg0, i32** %op3
	ld.d $t0, $fp, -40
	ld.d $t1, $fp, -24
	st.d $t1, $t0, 0
# %op4 = alloca i32
	addi.d $t0, $fp, -60
	st.d $t0, $t0, 4
# store i32 %arg1, i32* %op4
	ld.d $t0, $fp, -56
	ld.w $t1, $fp, -28
	st.w $t1, $t0, 0
# %op5 = alloca i32
	addi.d $t0, $fp, -76
	st.d $t0, $t0, 4
# store i32 %arg2, i32* %op5
	ld.d $t0, $fp, -72
	ld.w $t1, $fp, -32
	st.w $t1, $t0, 0
# %op6 = load i32, i32* %op4
	ld.d $t0, $fp, -56
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -80
# %op7 = icmp slt i32 %op6, 0
	ld.w $t0, $fp, -80
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -81
# br i1 %op7, label %label8, label %label9
	ld.b $t0, $fp, -81
	bnez $t0, .store_label8
	b .store_label9
.store_label8:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label9
	b .store_label9
.store_label9:
# %op10 = load i32*, i32** %op3
	ld.d $t0, $fp, -40
	ld.d $t0, $t0, 0
	st.d $t0, $fp, -96
# %op11 = getelementptr i32, i32* %op10, i32 %op6
	ld.d $t0, $fp, -96
	ld.w $t1, $fp, -80
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -104
# %op12 = load i32, i32* %op5
	ld.d $t0, $fp, -72
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -108
# store i32 %op12, i32* %op11
	ld.d $t0, $fp, -104
	ld.w $t1, $fp, -108
	st.w $t1, $t0, 0
# %op13 = load i32, i32* %op5
	ld.d $t0, $fp, -72
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -112
# ret i32 %op13
	ld.w $a0, $fp, -112
	b store_exit
store_exit:
	addi.d $sp, $sp, 112
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -192
.main_label_entry:
# %op0 = alloca [10 x i32]
	addi.d $t0, $fp, -64
	st.d $t0, $t0, 40
# %op1 = alloca i32
	addi.d $t0, $fp, -76
	st.d $t0, $t0, 4
# %op2 = alloca i32
	addi.d $t0, $fp, -92
	st.d $t0, $t0, 4
# store i32 0, i32* %op1
	ld.d $t0, $fp, -72
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# br label %label3
	b .main_label3
.main_label3:
# %op4 = load i32, i32* %op1
	ld.d $t0, $fp, -72
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -96
# %op5 = icmp slt i32 %op4, 10
	ld.w $t0, $fp, -96
	addi.w $t1, $zero, 10
	slt $t2, $t0, $t1
	st.b $t2, $fp, -97
# br i1 %op5, label %label6, label %label14
	ld.b $t0, $fp, -97
	bnez $t0, .main_label6
	b .main_label14
.main_label6:
# %op7 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 0
	ld.d $t0, $fp, -24
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -112
# %op8 = load i32, i32* %op1
	ld.d $t0, $fp, -72
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -116
# %op9 = load i32, i32* %op1
	ld.d $t0, $fp, -72
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -120
# %op10 = mul i32 %op9, 2
	ld.w $t0, $fp, -120
	addi.w $t1, $zero, 2
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -124
# %op11 = call i32 @store(i32* %op7, i32 %op8, i32 %op10)
	ld.d $a0, $fp, -112
	ld.w $a1, $fp, -116
	ld.w $a2, $fp, -124
	bl store
	st.w $a0, $fp, -128
# %op12 = load i32, i32* %op1
	ld.d $t0, $fp, -72
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -132
# %op13 = add i32 %op12, 1
	ld.w $t0, $fp, -132
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -136
# store i32 %op13, i32* %op1
	ld.d $t0, $fp, -72
	ld.w $t1, $fp, -136
	st.w $t1, $t0, 0
# br label %label3
	b .main_label3
.main_label14:
# store i32 0, i32* %op2
	ld.d $t0, $fp, -88
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# store i32 0, i32* %op1
	ld.d $t0, $fp, -72
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# br label %label15
	b .main_label15
.main_label15:
# %op16 = load i32, i32* %op1
	ld.d $t0, $fp, -72
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -140
# %op17 = icmp slt i32 %op16, 10
	ld.w $t0, $fp, -140
	addi.w $t1, $zero, 10
	slt $t2, $t0, $t1
	st.b $t2, $fp, -141
# br i1 %op17, label %label18, label %label22
	ld.b $t0, $fp, -141
	bnez $t0, .main_label18
	b .main_label22
.main_label18:
# %op19 = load i32, i32* %op2
	ld.d $t0, $fp, -88
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -148
# %op20 = load i32, i32* %op1
	ld.d $t0, $fp, -72
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -152
# %op21 = icmp slt i32 %op20, 0
	ld.w $t0, $fp, -152
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	st.b $t2, $fp, -153
# br i1 %op21, label %label24, label %label25
	ld.b $t0, $fp, -153
	bnez $t0, .main_label24
	b .main_label25
.main_label22:
# %op23 = load i32, i32* %op2
	ld.d $t0, $fp, -88
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -160
# call void @output(i32 %op23)
	ld.w $a0, $fp, -160
	bl output
# ret i32 0
	addi.w $a0, $zero, 0
	b main_exit
.main_label24:
# call void @neg_idx_except()
	bl neg_idx_except
# br label %label25
	b .main_label25
.main_label25:
# %op26 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 %op20
	ld.d $t0, $fp, -24
	addi.w $t1, $zero, 0
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	ld.w $t1, $fp, -152
	addi.d $t2, $zero, 4
	mul.d $t1, $t1, $t2
	add.d $t0, $t0, $t1
	st.d $t0, $fp, -168
# %op27 = load i32, i32* %op26
	ld.d $t0, $fp, -168
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -172
# %op28 = add i32 %op19, %op27
	ld.w $t0, $fp, -148
	ld.w $t1, $fp, -172
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -176
# store i32 %op28, i32* %op2
	ld.d $t0, $fp, -88
	ld.w $t1, $fp, -176
	st.w $t1, $t0, 0
# %op29 = load i32, i32* %op1
	ld.d $t0, $fp, -72
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -180
# %op30 = add i32 %op29, 1
	ld.w $t0, $fp, -180
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -184
# store i32 %op30, i32* %op1
	ld.d $t0, $fp, -72
	ld.w $t1, $fp, -184
	st.w $t1, $t0, 0
# br label %label15
	b .main_label15
main_exit:
	addi.d $sp, $sp, 192
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
