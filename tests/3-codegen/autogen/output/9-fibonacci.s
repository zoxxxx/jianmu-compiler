	.text
	.globl fibonacci
	.type fibonacci, @function
fibonacci:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -80
	st.w $a0, $fp, -20
.fibonacci_label_entry:
# %op1 = alloca i32
	addi.d $t0, $fp, -36
	st.d $t0, $t0, 4
# store i32 %arg0, i32* %op1
	ld.d $t0, $fp, -32
	ld.w $t1, $fp, -20
	st.w $t1, $t0, 0
# %op2 = load i32, i32* %op1
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -40
# %op3 = icmp eq i32 %op2, 0
	ld.w $t0, $fp, -40
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	slt $t3, $t1, $t0
	or $t2, $t2, $t3
	xori $t2, $t2, 1
	st.b $t2, $fp, -41
# br i1 %op3, label %label4, label %label5
	ld.b $t0, $fp, -41
	bnez $t0, .fibonacci_label4
	b .fibonacci_label5
.fibonacci_label4:
# ret i32 0
	addi.w $a0, $zero, 0
	b fibonacci_exit
.fibonacci_label5:
# %op6 = load i32, i32* %op1
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -48
# %op7 = icmp eq i32 %op6, 1
	ld.w $t0, $fp, -48
	addi.w $t1, $zero, 1
	slt $t2, $t0, $t1
	slt $t3, $t1, $t0
	or $t2, $t2, $t3
	xori $t2, $t2, 1
	st.b $t2, $fp, -49
# br i1 %op7, label %label9, label %label10
	ld.b $t0, $fp, -49
	bnez $t0, .fibonacci_label9
	b .fibonacci_label10
.fibonacci_label8:
# ret i32 0
	addi.w $a0, $zero, 0
	b fibonacci_exit
.fibonacci_label9:
# ret i32 1
	addi.w $a0, $zero, 1
	b fibonacci_exit
.fibonacci_label10:
# %op11 = load i32, i32* %op1
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -56
# %op12 = sub i32 %op11, 1
	ld.w $t0, $fp, -56
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -60
# %op13 = call i32 @fibonacci(i32 %op12)
	ld.w $a0, $fp, -60
	bl fibonacci
	st.w $a0, $fp, -64
# %op14 = load i32, i32* %op1
	ld.d $t0, $fp, -32
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -68
# %op15 = sub i32 %op14, 2
	ld.w $t0, $fp, -68
	addi.w $t1, $zero, 2
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -72
# %op16 = call i32 @fibonacci(i32 %op15)
	ld.w $a0, $fp, -72
	bl fibonacci
	st.w $a0, $fp, -76
# %op17 = add i32 %op13, %op16
	ld.w $t0, $fp, -64
	ld.w $t1, $fp, -76
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -80
# ret i32 %op17
	ld.w $a0, $fp, -80
	b fibonacci_exit
.fibonacci_label18:
# br label %label8
	b .fibonacci_label8
fibonacci_exit:
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
	addi.d $sp, $sp, -80
.main_label_entry:
# %op0 = alloca i32
	addi.d $t0, $fp, -28
	st.d $t0, $t0, 4
# %op1 = alloca i32
	addi.d $t0, $fp, -44
	st.d $t0, $t0, 4
# store i32 10, i32* %op0
	ld.d $t0, $fp, -24
	addi.w $t1, $zero, 10
	st.w $t1, $t0, 0
# store i32 0, i32* %op1
	ld.d $t0, $fp, -40
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# br label %label2
	b .main_label2
.main_label2:
# %op3 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -48
# %op4 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -52
# %op5 = icmp slt i32 %op3, %op4
	ld.w $t0, $fp, -48
	ld.w $t1, $fp, -52
	slt $t2, $t0, $t1
	st.b $t2, $fp, -53
# br i1 %op5, label %label6, label %label11
	ld.b $t0, $fp, -53
	bnez $t0, .main_label6
	b .main_label11
.main_label6:
# %op7 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -60
# %op8 = call i32 @fibonacci(i32 %op7)
	ld.w $a0, $fp, -60
	bl fibonacci
	st.w $a0, $fp, -64
# call void @output(i32 %op8)
	ld.w $a0, $fp, -64
	bl output
# %op9 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -68
# %op10 = add i32 %op9, 1
	ld.w $t0, $fp, -68
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -72
# store i32 %op10, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t1, $fp, -72
	st.w $t1, $t0, 0
# br label %label2
	b .main_label2
.main_label11:
# ret i32 0
	addi.w $a0, $zero, 0
	b main_exit
main_exit:
	addi.d $sp, $sp, 80
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
