; ModuleID = 'calculator'
declare void @output(i32)

define i32 @main() {
label_entry:
  %op0 = mul i32 2, 3
  %op1 = sub i32 %op0, 4
  %op2 = mul i32 %op1, 2
  %op3 = add i32 1, %op2
  call void @output(i32 %op3)
  ret i32 0
}
