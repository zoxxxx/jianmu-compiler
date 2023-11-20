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
	addi.d $sp, $sp, -96
.returnToZeroSteps_label_entry:
# %op0 = alloca i32
	addi.d $t0, $fp, -28
	st.d $t0, $t0, 4
# %op1 = alloca i32
	addi.d $t0, $fp, -44
	st.d $t0, $t0, 4
# store i32 0, i32* %op0
	ld.d $t0, $fp, -24
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# store i32 0, i32* %op1
	ld.d $t0, $fp, -40
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# br label %label2
	b .returnToZeroSteps_label2
.returnToZeroSteps_label2:
# %op3 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -48
# %op4 = icmp slt i32 %op3, 20
	ld.w $t0, $fp, -48
	addi.w $t1, $zero, 20
	slt $t2, $t0, $t1
	st.b $t2, $fp, -49
# br i1 %op4, label %label5, label %label8
	ld.b $t0, $fp, -49
	bnez $t0, .returnToZeroSteps_label5
	b .returnToZeroSteps_label8
.returnToZeroSteps_label5:
# %op6 = call i32 @randBin()
	bl randBin
	st.w $a0, $fp, -56
# %op7 = icmp ne i32 %op6, 0
	ld.w $t0, $fp, -56
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	slt $t3, $t1, $t0
	or $t2, $t2, $t3
	st.b $t2, $fp, -57
# br i1 %op7, label %label9, label %label12
	ld.b $t0, $fp, -57
	bnez $t0, .returnToZeroSteps_label9
	b .returnToZeroSteps_label12
.returnToZeroSteps_label8:
# ret i32 20
	addi.w $a0, $zero, 20
	b returnToZeroSteps_exit
.returnToZeroSteps_label9:
# %op10 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -64
# %op11 = add i32 %op10, 1
	ld.w $t0, $fp, -64
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -68
# store i32 %op11, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t1, $fp, -68
	st.w $t1, $t0, 0
# br label %label15
	b .returnToZeroSteps_label15
.returnToZeroSteps_label12:
# %op13 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -72
# %op14 = sub i32 %op13, 1
	ld.w $t0, $fp, -72
	addi.w $t1, $zero, 1
	sub.w $t2, $t0, $t1
	st.w $t2, $fp, -76
# store i32 %op14, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t1, $fp, -76
	st.w $t1, $t0, 0
# br label %label15
	b .returnToZeroSteps_label15
.returnToZeroSteps_label15:
# %op16 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -80
# %op17 = add i32 %op16, 1
	ld.w $t0, $fp, -80
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -84
# store i32 %op17, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t1, $fp, -84
	st.w $t1, $t0, 0
# %op18 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -88
# %op19 = icmp eq i32 %op18, 0
	ld.w $t0, $fp, -88
	addi.w $t1, $zero, 0
	slt $t2, $t0, $t1
	slt $t3, $t1, $t0
	or $t2, $t2, $t3
	xori $t2, $t2, 1
	st.b $t2, $fp, -89
# br i1 %op19, label %label20, label %label22
	ld.b $t0, $fp, -89
	bnez $t0, .returnToZeroSteps_label20
	b .returnToZeroSteps_label22
.returnToZeroSteps_label20:
# %op21 = load i32, i32* %op1
	ld.d $t0, $fp, -40
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -96
# ret i32 %op21
	ld.w $a0, $fp, -96
	b returnToZeroSteps_exit
.returnToZeroSteps_label22:
# br label %label2
	b .returnToZeroSteps_label2
returnToZeroSteps_exit:
	addi.d $sp, $sp, 96
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
	.globl main
	.type main, @function
main:
	st.d $ra, $sp, -8
	st.d $fp, $sp, -16
	addi.d $fp, $sp, 0
	addi.d $sp, $sp, -48
.main_label_entry:
# %op0 = alloca i32
	addi.d $t0, $fp, -28
	st.d $t0, $t0, 4
# store i32 0, i32* %op0
	ld.d $t0, $fp, -24
	addi.w $t1, $zero, 0
	st.w $t1, $t0, 0
# store i32 3407, i32* @seed
	la.local $t0, seed
	lu12i.w $t1, 0
	ori $t1, $t1, 3407
	st.w $t1, $t0, 0
# br label %label1
	b .main_label1
.main_label1:
# %op2 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -32
# %op3 = icmp slt i32 %op2, 20
	ld.w $t0, $fp, -32
	addi.w $t1, $zero, 20
	slt $t2, $t0, $t1
	st.b $t2, $fp, -33
# br i1 %op3, label %label4, label %label8
	ld.b $t0, $fp, -33
	bnez $t0, .main_label4
	b .main_label8
.main_label4:
# %op5 = call i32 @returnToZeroSteps()
	bl returnToZeroSteps
	st.w $a0, $fp, -40
# call void @output(i32 %op5)
	ld.w $a0, $fp, -40
	bl output
# %op6 = load i32, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t0, $t0, 0
	st.w $t0, $fp, -44
# %op7 = add i32 %op6, 1
	ld.w $t0, $fp, -44
	addi.w $t1, $zero, 1
	add.w $t2, $t0, $t1
	st.w $t2, $fp, -48
# store i32 %op7, i32* %op0
	ld.d $t0, $fp, -24
	ld.w $t1, $fp, -48
	st.w $t1, $t0, 0
# br label %label1
	b .main_label1
.main_label8:
# ret i32 0
	addi.w $a0, $zero, 0
	b main_exit
main_exit:
	addi.d $sp, $sp, 48
	ld.d $ra, $sp, -8
	ld.d $fp, $sp, -16
	jr $ra
