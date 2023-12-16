	.text
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -32
.main_label_entry:
# br label %label0
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -20
	b .main_label0
.main_label0:
# %op1 = phi i32 [ 0, %label_entry ], [ %op4, %label3 ]
# %op2 = icmp slt i32 %op1, 10
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 10
	slt $t2, $t0, $t1
	st.b $t2, $fp, -21
# br i1 %op2, label %label3, label %label5
	ld.b $t0, $fp, -21
	bnez $t0, .main_label3
	b .main_label5
.main_label3:
# call void @output(i32 %op1)
	ld.w $a0, $fp, -20
	bl output
# %op4 = add i32 %op1, 1
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -28
# br label %label0
	ld.w $t0, $fp, -28
	st.w $t0, $fp, -20
	b .main_label0
.main_label5:
# ret i32 0
	addi.w $a0, $zero, 0
	b main_exit
main_exit:
	addi.d $sp, $sp, 32
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
