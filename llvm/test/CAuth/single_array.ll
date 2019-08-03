; RUN: opt -cauth=arr -cauth-ir-arrays -S < %s | FileCheck %s

; CHECK-LABEL: @simple
; CHECK:  %cauth_alloc = alloca i64
; CHECK:  %0 = call i64 @llvm.ca.pacga(i64 [[FUNCID:[0-9]+]])
; CHECK:  store i64 %0, i64* %cauth_alloc, align 8
; CHECK:  %a = alloca [32 x i8]
; CHECK:  %arraydecay1 = getelementptr inbounds [32 x i8], [32 x i8]* %a, i32 0, i32 0
; CHECK:  call void @expose(i8* %arraydecay1)
; CHECK:  %1 = load i64, i64* %cauth_alloc
; CHECK:  %2 = call i64 @llvm.ca.pacga(i64 [[FUNCID]])
; CHECK:  %cmp = icmp eq i64 %1, %2
; CHECK:  br i1 %cmp, label %cauth.ret, label %cauth.fail
; CHECK: cauth.ret:
; CHECK:  ret void
; CHECK: cauth.fail:
; CHECK:  %printfCall = call i32 (...) @printf([41 x i8]* @__canary_chk_fail)
; CHECK:  call void @exit(i32 1)
; CHECK:  ret void
define hidden void @simple() {
entry:
  %a = alloca [32 x i8]
  %arraydecay1 = getelementptr inbounds [32 x i8], [32 x i8]* %a, i32 0, i32 0
  call void @expose(i8* %arraydecay1)
  ret void
}

declare hidden void @expose(i8* %str)
