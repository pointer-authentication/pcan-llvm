; Incomplete test, idea was to test stack ordering.
;
; RUN: llc -O=3 -verify-machineinstrs -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -cauth=arr < %s | FileCheck %s

; CHECK-LABEL: @tester3
; // Make sure we and store the canary
; CHECK: mov [[MOD1:x[0-9]+]], sp
; CHECK: movk [[MOD1]], #{{[0-9]+}}, lsl #48
; CHECK: pacga [[CAN1:x[0-9]+]], [[MOD1]], {{x[0-9]+}}
; CHECK: {{str|stp x30,}} [[CAN1]]
; CHECK: bl printer32
; CHECK: bl printer32
; CHECK: bl printer32
; // Make sure the canary is loaded!
; CHECK: {{ldur|ldr|ldp .*}} [[CAN2:x[0-9]+]]
; // Make sure the reference canary is re-created from scratch!
; CHECK: mov [[MOD2:x[0-9]+]], sp
; CHECK: movk [[MOD2]], #{{[0-9]+}}, lsl #48
; CHECK: pacga [[REF_CAN:x[0-9]+]], [[MOD2]], {{x[0-9]+}}
; // Finally, just check the canary...
; CHECK: cmp [[CAN2]], [[REF_CAN]]
; CHECK: ret
define hidden void @tester3() local_unnamed_addr #0 {
entry:
  %tricky1 = alloca i32, align 4
  %a = alloca [54 x i8], align 1
  %tricky2 = alloca i32, align 4
  %tricky3 = alloca i32, align 4
  %0 = bitcast i32* %tricky1 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %0) #6
  store i32 34, i32* %tricky1, align 4, !tbaa !7
  %1 = getelementptr inbounds [54 x i8], [54 x i8]* %a, i64 0, i64 0
  call void @llvm.lifetime.start.p0i8(i64 54, i8* nonnull %1) #6
  call void @llvm.memset.p0i8.i64(i8* nonnull align 1 %1, i8 0, i64 54, i1 false)
  %2 = bitcast i32* %tricky2 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %2) #6
  store i32 34, i32* %tricky2, align 4, !tbaa !7
  %3 = bitcast i32* %tricky3 to i8*
  call void @llvm.lifetime.start.p0i8(i64 4, i8* nonnull %3) #6
  store i32 34, i32* %tricky3, align 4, !tbaa !7
  call void @do_copy(i8* nonnull %1, i8* getelementptr inbounds ([12 x i8], [12 x i8]* @.str.3, i64 0, i64 0))
  %4 = tail call { i64, i64 } asm "mov $0, sp;mov $1, x29;", "=r,=r"() #7, !srcloc !11
  %asmresult = extractvalue { i64, i64 } %4, 0
  %asmresult1 = extractvalue { i64, i64 } %4, 1
  store i64 %asmresult, i64* @sp, align 8, !tbaa !3
  store i64 %asmresult1, i64* @fp, align 8, !tbaa !3
  call void @printer32(i32* nonnull %tricky1)
  call void @printer(i8* nonnull %1)
  call void @printer32(i32* nonnull %tricky2)
  call void @printer32(i32* nonnull %tricky3)
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %3) #6
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %2) #6
  call void @llvm.lifetime.end.p0i8(i64 54, i8* nonnull %1) #6
  call void @llvm.lifetime.end.p0i8(i64 4, i8* nonnull %0) #6
  ret void
}

@fp = hidden local_unnamed_addr global i64 0, align 8
@sp = hidden local_unnamed_addr global i64 0, align 8
@.str = private unnamed_addr constant [11 x i8] c"%lx -> %s\0A\00", align 1
@.str.1 = private unnamed_addr constant [12 x i8] c"%lx -> %lu\0A\00", align 1
@.str.2 = private unnamed_addr constant [11 x i8] c"%lx -> %u\0A\00", align 1
@.str.3 = private unnamed_addr constant [12 x i8] c"Hello World\00", align 1
@.str.4 = private unnamed_addr constant [13 x i8] c"0x%lx -> SP\0A\00", align 1
@.str.5 = private unnamed_addr constant [13 x i8] c"0x%lx -> FP\0A\00", align 1

declare void @do_copy(i8* %dst, i8* nocapture readonly %src)
declare void @printer(i8* %str)
declare void @printer64(i64* %str)
declare void @printer32(i32* %str)

declare void @llvm.memset.p0i8.i64(i8* nocapture writeonly, i8, i64, i1)
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture)
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture)

attributes #0 = { noinline nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { argmemonly nounwind }
attributes #2 = { argmemonly nounwind readonly "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #4 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { nounwind readonly }
attributes #6 = { nounwind }
attributes #7 = { nounwind readnone }

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"PIC Level", i32 2}
!2 = !{!"clang version 8.0.0 "}
!3 = !{!4, !4, i64 0}
!4 = !{!"long", !5, i64 0}
!5 = !{!"omnipotent char", !6, i64 0}
!6 = !{!"Simple C/C++ TBAA"}
!7 = !{!8, !8, i64 0}
!8 = !{!"int", !5, i64 0}
!9 = !{i32 1157}
!10 = !{i32 1583}
!11 = !{i32 1915}
