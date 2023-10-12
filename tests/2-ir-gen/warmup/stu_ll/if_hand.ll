define i32 @main() {
label_entry:
  %op0 = alloca float
  store float 0x40163851e0000000, float* %op0
  %op1 = load float, float* %op0
  %op2 = fcmp ugt float %op1, 0x3ff0000000000000
  br i1 %op2, label %label_trueBB, label %label_falseBB
label_trueBB:                                                ; preds = %label_entry
  ret i32 233
label_falseBB:                                                ; preds = %label_entry
  ret i32 0
}
