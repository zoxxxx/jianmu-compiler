# Global variables
	.text
	.section .bss, "aw", @nobits
	.globl seed
	.type seed, @object
	.size seed, 4
seed:
	.space 4
	.text
	.globl randomLCG
	.type randomLCG, @function
randomLCG:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -32
.randomLCG_label_entry:
# %op0 = load i32, i32* @seed
	la.local $t0, seed
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -20
# %op1 = mul i32 %op0, 1103515245
	ld.w $t0, $fp, -20
	lu12i.w $t1, 269412
	ori $t1, $t1, 3693
	mul.w $t2, $t0, $t1
	st.w $t2, $fp, -24
# %op2 = add i32 %op1, 12345
	ld.w $t0, $fp, -24
	lu12i.w $t1, 3
	ori $t1, $t1, 57
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -28
# store i32 %op2, i32* @seed
	la.local $t0, seed
	ld.w $t1, $fp, -28
	st.w $t1, $t0, 0
# %op3 = load i32, i32* @seed
	la.local $t0, seed
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -32
# ret i32 %op3
	ld.w $a0, $fp, -32
	b randomLCG_exit
randomLCG_exit:
	addi.d $sp, $sp, 32
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl randBin
	.type randBin, @function
randBin:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -32
.randBin_label_entry:
# %op0 = call i32 @randomLCG()
	bl randomLCG
	st.w $a0, $fp, -20
# %op1 = icmp sgt i32 %op0, 0
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 0
	slt $t2, $t1, $t0
	st.b $t2, $fp, -21
# br i1 %op1, label %label2, label %label3
	ld.b $t0, $fp, -21
	bnez $t0, .randBin_label2
	b .randBin_label3
.randBin_label2:
# ret i32 1
	addi.w $a0, $zero, 1
	b randBin_exit
.randBin_label3:
# ret i32 0
	addi.w $a0, $zero, 0
	b randBin_exit
.randBin_label4:
# ret i32 0
	addi.w $a0, $zero, 0
	b randBin_exit
randBin_exit:
	addi.d $sp, $sp, 32
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl returnToZeroSteps
	.type returnToZeroSteps, @function
returnToZeroSteps:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -64
.returnToZeroSteps_label_entry:
# br label %label0
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -20
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -24
	b .returnToZeroSteps_label0
.returnToZeroSteps_label0:
# %op1 = phi i32 [ 0, %label_entry ], [ %op14, %label17 ]
# %op2 = phi i32 [ 0, %label_entry ], [ %op13, %label17 ]
# %op3 = icmp slt i32 %op1, 20
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 20
	slt $t2, $t0, $t1
	st.b $t2, $fp, -25
# br i1 %op3, label %label4, label %label7
	ld.b $t0, $fp, -25
	bnez $t0, .returnToZeroSteps_label4
	b .returnToZeroSteps_label7
.returnToZeroSteps_label4:
# %op5 = call i32 @randBin()
	bl randBin
	st.w $a0, $fp, -32
# %op6 = icmp ne i32 %op5, 0
	ld.w $t0, $fp, -32
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	slt $t3, $t1, $t0
	or $t2, $t2, $t3
	st.b $t2, $fp, -33
# br i1 %op6, label %label8, label %label10
	ld.b $t0, $fp, -33
	bnez $t0, .returnToZeroSteps_label8
	b .returnToZeroSteps_label10
.returnToZeroSteps_label7:
# ret i32 20
	addi.w $a0, $zero, 20
	b returnToZeroSteps_exit
.returnToZeroSteps_label8:
# %op9 = add i32 %op2, 1
	ld.w $t0, $fp, -24
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -40
# br label %label12
	ld.w $t0, $fp, -40
	st.w $t0, $fp, -48
	b .returnToZeroSteps_label12
.returnToZeroSteps_label10:
# %op11 = sub i32 %op2, 1
	ld.w $t0, $fp, -24
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -44
# br label %label12
	ld.w $t0, $fp, -44
	st.w $t0, $fp, -48
	b .returnToZeroSteps_label12
.returnToZeroSteps_label12:
# %op13 = phi i32 [ %op9, %label8 ], [ %op11, %label10 ]
# %op14 = add i32 %op1, 1
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -52
# %op15 = icmp eq i32 %op13, 0
	ld.w $t0, $fp, -48
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	slt $t3, $t1, $t0
	or $t2, $t2, $t3
	xori $t2, $t2, 1
	st.b $t2, $fp, -53
# br i1 %op15, label %label16, label %label17
	ld.b $t0, $fp, -53
	bnez $t0, .returnToZeroSteps_label16
	b .returnToZeroSteps_label17
.returnToZeroSteps_label16:
# ret i32 %op14
	ld.w $a0, $fp, -52
	b returnToZeroSteps_exit
.returnToZeroSteps_label17:
# br label %label0
	ld.w $t0, $fp, -52
	st.w $t0, $fp, -20
	ld.w $t0, $fp, -48
	st.w $t0, $fp, -24
	b .returnToZeroSteps_label0
returnToZeroSteps_exit:
	addi.d $sp, $sp, 64
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -32
.main_label_entry:
# store i32 3407, i32* @seed
	la.local $t0, seed
	lu12i.w $t1, 0
	ori $t1, $t1, 3407
	st.w $t1, $t0, 0
# br label %label0
	addi.w $t0, $zero, 0
	st.w $t0, $fp, -20
	b .main_label0
.main_label0:
# %op1 = phi i32 [ 0, %label_entry ], [ %op5, %label3 ]
# %op2 = icmp slt i32 %op1, 20
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 20
	slt $t2, $t0, $t1
	st.b $t2, $fp, -21
# br i1 %op2, label %label3, label %label6
	ld.b $t0, $fp, -21
	bnez $t0, .main_label3
	b .main_label6
.main_label3:
# %op4 = call i32 @returnToZeroSteps()
	bl returnToZeroSteps
	st.w $a0, $fp, -28
# call void @output(i32 %op4)
	ld.w $a0, $fp, -28
	bl output
# %op5 = add i32 %op1, 1
	ld.w $t0, $fp, -20
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -32
# br label %label0
	ld.w $t0, $fp, -32
	st.w $t0, $fp, -20
	b .main_label0
.main_label6:
# ret i32 0
	addi.w $a0, $zero, 0
	b main_exit
main_exit:
	addi.d $sp, $sp, 32
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
