; RUN: opt -cauth=arr -cauth-ir-arrays -S < %s | FileCheck %s

; CHECK-LABEL: @simple
; CHECK-DAG:  %cauth_alloc = alloca i64
; CHECK-DAG:  [[MOD1:%[0-9]+]] = call i64 @llvm.cauth.pro.mod()
; CHECK:  %pga = call i64 @llvm.ca.pacga(i64 [[MOD1]])
; CHECK:  store i64 %pga, i64* %cauth_alloc, align 8
; CHECK:  %a = alloca [32 x i8]
; CHECK:  %arraydecay1 = getelementptr inbounds [32 x i8], [32 x i8]* %a, i32 0, i32 0
; CHECK:  call void @expose(i8* %arraydecay1)

; CHECK-DAG:  [[MOD2:%[0-9]+]] = call i64 @llvm.cauth.epi.mod()
; CHECK-DAG:  [[CAN:%[0-9]+]] = load i64, i64* %cauth_alloc
; CHECK-DAG:  %ega = call i64 @llvm.ca.pacga(i64 [[MOD2]])
; CHECK:  %cmp = icmp eq i64 [[CAN]], %ega
; CHECK:  br i1 %cmp, label %cauth.ret, label %cauth.fail
; CHECK: cauth.ret:
; CHECK:  ret void
; CHECK: cauth.fail:
; CHECK:  @printf({{.*}}@__canary_chk_fail
; CHECK:  call void @exit
; CHECK:  ret void
define hidden void @simple() {
entry:
  %a = alloca [32 x i8]
  %arraydecay1 = getelementptr inbounds [32 x i8], [32 x i8]* %a, i32 0, i32 0
  call void @expose(i8* %arraydecay1)
  ret void
}

declare hidden void @expose(i8* %str)
