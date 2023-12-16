	.text
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -48
.main_label_entry:
# br label %label0
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -20
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -24
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -28
	b .main_label0
.main_label0:
# %op1 = phi i32 [ 0, %label_entry ], [ %op6, %label5 ]
# %op2 = phi i32 [ 0, %label_entry ], [ %op7, %label5 ]
# %op3 = phi i32 [ 0, %label_entry ], [ %op2, %label5 ]
# %op4 = icmp sle i32 %op1, 1
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 1
	slt $t2, $t1, $t0
	xori $t2, $t2, 1
	st.b $t2, $fp, -29
# br i1 %op4, label %label5, label %label8
	ld.b $t0, $fp, -29
	bnez $t0, .main_label5
	b .main_label8
.main_label5:
# %op6 = add i32 %op1, 1
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -36
# %op7 = add i32 %op2, 1
	ld.w $t0, $fp, -24
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -40
# br label %label0
	ld.w $t0, $fp, -36
	st.w $t0, $fp, -20
	ld.w $t0, $fp, -40
	st.w $t0, $fp, -24
	ld.w $t0, $fp, -24
	st.w $t0, $fp, -28
	b .main_label0
.main_label8:
# call void @output(i32 %op3)
	ld.w $a0, $fp, -28
	bl output
# ret i32 0
	addi.w $a0, $zero, 0
	b main_exit
main_exit:
	addi.d $sp, $sp, 48
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
