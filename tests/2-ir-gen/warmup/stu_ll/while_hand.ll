define i32 @main() {
label_entry:
  %op0 = alloca i32
  %op1 = alloca i32
  store i32 10, i32* %op0
  store i32 0, i32* %op1
  br label %label_cmpBB
label_cmpBB:                                                ; preds = %label_entry, %label_whileBB
  %op2 = load i32, i32* %op1
  %op3 = icmp slt i32 %op2, 10
  br i1 %op3, label %label_whileBB, label %label_retBB
label_whileBB:                                                ; preds = %label_cmpBB
  %op4 = add i32 %op2, 1
  store i32 %op4, i32* %op1
  %op5 = load i32, i32* %op0
  %op6 = add i32 %op5, %op4
  store i32 %op6, i32* %op0
  br label %label_cmpBB
label_retBB:                                                ; preds = %label_cmpBB
  %op7 = load i32, i32* %op0
  ret i32 %op7
}
