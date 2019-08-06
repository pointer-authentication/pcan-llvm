; RUN: llc -verify-machineinstrs -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -cauth=arr < %s | FileCheck %s

; CHECK-LABEL: @simple
; CHECK: mov [[MOD1:x[0-9]+]], sp
; CHECK: movk [[MOD1]], #{{[0-9]+}}, lsl #48
; CHECK: pacga [[C1:x[0-9]+]], [[MOD1]], [[MOD1]]
; CHECK: pacda [[C2:x[0-9]+]], [[MOD1]]
; CHECK-DAG: st{{[rp].*}}[[C1]]
; CHECK-DAG: st{{[rp].*}}[[C2]]

; CHECK: bl expose

; CHECK-DAG: mov [[MOD2:x[0-9]+]], sp
; CHECK-DAG: movk [[MOD2]], #{{[0-9]+}}, lsl #48
; CHECK: autda [[SC2:x[0-9]+]], [[MOD2]]
; CHECK-DAG: ldr [[SC1:x[0-9]+]], {{\[}}[[SC2]]{{\]}}
; // Make sure we always recreate the canary here!
; CHECK-DAG: pacga [[REF_CAN:x[0-9]+]], [[MOD2]], [[MOD2]]
; CHECK: cmp [[SC1]], [[REF_CAN]]
; CHECK: ret
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
