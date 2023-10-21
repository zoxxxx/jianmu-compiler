; ModuleID = 'cminus'
source_filename = "/home/zox/compiler/2023ustc-jianmu-compiler/tests/2-ir-gen/autogen/testcases/lv2/return_in_middle2.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define i32 @result() {
label_entry:
  %op0 = alloca i32
  store i32 10, i32* %op0
  br label %label1
label1:                                                ; preds = %label_entry
  %op2 = load i32, i32* %op0
  %op3 = icmp sgt i32 %op2, 0
  br i1 %op3, label %label4, label %label5
label4:                                                ; preds = %label1
  ret i32 0
label5:                                                ; preds = %label1
  call void @output(i32 4)
  ret i32 1
}
define void @main() {
label_entry:
  %op0 = call i32 @result()
  call void @output(i32 %op0)
  ret void
}
