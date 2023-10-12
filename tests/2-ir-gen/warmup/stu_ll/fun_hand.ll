define i32 @callee(i32 %arg0) {
label_entry:
  %op1 = mul i32 %arg0, 2
  ret i32 %op1
}
define i32 @main() {
label_entry:
  %op0 = call i32 @callee(i32 110)
  ret i32 %op0
}
