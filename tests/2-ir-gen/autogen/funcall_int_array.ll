; ModuleID = 'cminus'
source_filename = "/home/zox/compiler/2023ustc-jianmu-compiler/tests/2-ir-gen/autogen/testcases/lv2/funcall_int_array.cminus"

declare i32 @input()

declare void @output(i32)

declare void @outputFloat(float)

declare void @neg_idx_except()

define void @test(i32* %arg0) {
label_entry:
  %op1 = icmp slt i32 3, 0
  br i1 %op1, label %label2, label %label3
label2:                                                ; preds = %label_entry
  call void @neg_idx_except()
  br label %label3
label3:                                                ; preds = %label_entry, %label2
  %op4 = getelementptr i32, i32* %arg0, i32 0, i32 3
  %op5 = load i32, i32* %op4
  call void @output(i32 %op5)
  ret void
}
define void @main() {
label_entry:
  %op0 = alloca [10 x i32]
  %op1 = icmp slt i32 3, 0
  br i1 %op1, label %label2, label %label3
label2:                                                ; preds = %label_entry
  call void @neg_idx_except()
  br label %label3
label3:                                                ; preds = %label_entry, %label2
  %op4 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 3
  store i32 10, i32* %op4
  %op5 = getelementptr [10 x i32], [10 x i32]* %op0, i32 0, i32 0
  call void @test(i32* %op5)
  ret void
}
