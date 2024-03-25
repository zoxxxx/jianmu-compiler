
	.text
	.section .bss, "aw", @nobits
	.globl a
	.type a, @object
	.size a, 4
a:
	.space 4
	.data
	.text
	.globl func
	.type func, @function
func_label_entry:
	ori $t8, $zero, 1
	sub.w $a0, $t7, $t8
	b func_exit
func:
	ori $s8, $fp, 0
	ori $fp, $sp, 0
	b func_label_entry
func_exit:
	ori $sp, $fp, 0
	ori $fp, $s8, 0
	jr $ra
	.globl main
	.type main, @function
main_label_entry:
	ori $t8, $zero, 10
	la.local $t8, a
	st.w $t8, $t8, 0
	la.local $t8, a
	ld.w $a0, $t8, 0
	bl func
	b main_exit
main:
	st.d $fp, $sp, -16
	ori $fp, $sp, 0
	st.d $ra, $fp, -8
	b main_label_entry
main_exit:
	ld.d $ra, $fp, -8
	ori $sp, $fp, 0
	ld.d $fp, $sp, -16
	jr $ra


v_25 v_27
