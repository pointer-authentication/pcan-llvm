; RUN: opt -cauth=arr -cauth-ir-arrays -S < %s | FileCheck %s

; CHECK-LABEL: @simple
; CHECK: entry
; CHECK:   [[MOD1:%[a-z0-9_]+]] = call i64 @llvm.cauth.pro.mod()
; CHECK:   [[PCAN1:%[a-z0-9_]+]] = call i64 @llvm.ca.pacga(i64 [[MOD1]])
; CHECK:   store i64 [[PCAN1]], i64* [[ACAN1:%[a-z0-9_]+]]
; CHECK:   [[PCAN2:%[a-z0-9_]+]] = call i64* @llvm.ca.pacda.p0i64(i64* [[ACAN1]], i64 [[MOD1]])
; CHECK:   store i64* [[PCAN2]], i64** [[ACAN2:%[a-z0-9_]+]]

; CHECK:   %arraydecay1 = getelementptr inbounds [32 x i8], [32 x i8]* %a, i32 0, i32 0
; CHECK:   %arraydecay2 = getelementptr inbounds [32 x i8], [32 x i8]* %b, i32 0, i32 0
; CHECK:   call void @expose(i8* %arraydecay1, i8* %arraydecay2)

; CHECK-DAG:  [[MOD2:%[a-z0-9_]+]] = call i64 @llvm.cauth.epi.mod()
; CHECK-DAG:  [[ECAN1:%[a-z0-9_]+]] = load i64*, i64** [[ACAN2]]
; CHECK:   [[AECAN2:%[a-z0-9_]+]] = call i64* @llvm.ca.autda.p0i64(i64* [[ECAN1]], i64 [[MOD2]])
; CHECK:   [[ECAN2:%[a-z0-9_]+]] = load i64, i64* [[AECAN2]]
; CHECK:   [[REFCAN:%[a-z0-9_]+]] = call i64 @llvm.ca.pacga(i64 [[MOD2]])
; CHECK:   %cmp = icmp eq i64 [[ECAN2]], [[REFCAN]]
; CHECK    br i1 %cmp, label %cauth.ret, label %cauth.fail
; CHECK: cauth.ret
; CHECK: cauth.fail
define hidden void @simple() {
entry:
  %a = alloca [32 x i8]
  %b = alloca [32 x i8]
  %arraydecay1 = getelementptr inbounds [32 x i8], [32 x i8]* %a, i32 0, i32 0
  %arraydecay2 = getelementptr inbounds [32 x i8], [32 x i8]* %b, i32 0, i32 0
  call void @expose(i8* %arraydecay1, i8* %arraydecay2)
  ret void
}

declare hidden void @expose(i8*, i8*)
