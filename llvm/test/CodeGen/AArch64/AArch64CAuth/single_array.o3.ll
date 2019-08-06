; RUN: llc -O=3 -verify-machineinstrs -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -cauth=arr < %s | FileCheck %s

; CHECK-LABEL: @simple
; // Make sure we and store the canary
; CHECK: mov	[[MOD1:x[0-9]+]], sp
; CHECK: movk [[MOD1]], #{{[0-9]+}}, lsl #48
; CHECK: pacga [[CAN1:x[0-9]+]], [[MOD1]], {{x[0-9]+}}
; CHECK: {{stur|str|stp x30,}} [[CAN1]]
; CHECK: bl expose
; // Make sure the canary is loaded!
; CHECK: {{ldur|ldr|ldp .*}} [[CAN2:x[0-9]+]]
; // Make sure the reference canary is re-created from scratch!
; CHECK: mov [[MOD2:x[0-9]+]], sp
; CHECK: movk [[MOD2]], #{{[0-9]+}}, lsl #48
; CHECK: pacga [[REF_CAN:x[0-9]+]], [[MOD2]], {{x[0-9]+}}
; // Finally, just check the canary...
; CHECK: cmp [[CAN2]], [[REF_CAN]]
; CHECK: ret
define hidden void @simple() local_unnamed_addr #0 {
entry:
  %a = alloca [32 x i8], align 1
  %0 = getelementptr inbounds [32 x i8], [32 x i8]* %a, i64 0, i64 0
  call void @llvm.lifetime.start.p0i8(i64 32, i8* nonnull %0) #5
  call void @llvm.memset.p0i8.i64(i8* nonnull align 1 %0, i8 0, i64 32, i1 false)
  call void @expose(i8* nonnull %0)
  call void @llvm.lifetime.end.p0i8(i64 32, i8* nonnull %0) #5
  ret void
}

declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #1
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #1
declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1) #1

declare hidden void @expose(i8*)

attributes #0 = { noinline nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind }
attributes #2 = { argmemonly nounwind readonly "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nounwind }
attributes #6 = { nounwind readonly }
