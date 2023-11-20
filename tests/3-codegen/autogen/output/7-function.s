	.text
	.globl min
	.type min, @function
min:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -80
	st.w $a0, $fp, -20
	st.w $a1, $fp, -24
.min_label_entry:
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
# %op6 = icmp sle i32 %op4, %op5
	ld.w $t0, $fp, -56
	ld.w $t1, $fp, -60
	slt $t2, $t1, $t0
	xori $t2, $t2, 1
	st.b $t2, $fp, -61
# br i1 %op6, label %label7, label %label9
	ld.b $t0, $fp, -61
	bnez $t0, .min_label7
	b .min_label9
.min_label7:
# %op8 = load i32, i32* %op2
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -68
# ret i32 %op8
	ld.w $a0, $fp, -68
	b min_exit
.min_label9:
# %op10 = load i32, i32* %op3
	ld.d $t0, $fp, -48
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -72
# ret i32 %op10
	ld.w $a0, $fp, -72
	b min_exit
.min_label11:
# ret i32 0
	addi.w $a0, $zero, 0
	b min_exit
min_exit:
	addi.d $sp, $sp, 80
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -96
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
# store i32 11, i32* %op0
	ld.d $t0, $fp, -24
	addi.w $t1, $zero, 11
	st.w $t1, $t0, 0
# store i32 22, i32* %op1
	ld.d $t0, $fp, -40
	addi.w $t1, $zero, 22
	st.w $t1, $t0, 0
# store i32 33, i32* %op2
	ld.d $t0, $fp, -56
	addi.w $t1, $zero, 33
	st.w $t1, $t0, 0
# %op3 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -64
# %op4 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -68
# %op5 = call i32 @min(i32 %op3, i32 %op4)
	ld.w $a0, $fp, -64
	ld.w $a1, $fp, -68
	bl min
	st.w $a0, $fp, -72
# call void @output(i32 %op5)
	ld.w $a0, $fp, -72
	bl output
# %op6 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -76
# %op7 = load i32, i32* %op2
	ld.d $t0, $fp, -56
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -80
# %op8 = call i32 @min(i32 %op6, i32 %op7)
	ld.w $a0, $fp, -76
	ld.w $a1, $fp, -80
	bl min
	st.w $a0, $fp, -84
# call void @output(i32 %op8)
	ld.w $a0, $fp, -84
	bl output
# %op9 = load i32, i32* %op2
	ld.d $t0, $fp, -56
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -88
# %op10 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -92
# %op11 = call i32 @min(i32 %op9, i32 %op10)
	ld.w $a0, $fp, -88
	ld.w $a1, $fp, -92
	bl min
	st.w $a0, $fp, -96
# call void @output(i32 %op11)
	ld.w $a0, $fp, -96
	bl output
# ret i32 0
	addi.w $a0, $zero, 0
	b main_exit
main_exit:
	addi.d $sp, $sp, 96
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
