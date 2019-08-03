; RUN: llc -verify-machineinstrs -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -cauth=arr < %s | FileCheck %s

; CHECK-LABEL: @simple
; // Make sure we and store the canary
; CHECK: mov	[[MOD1:x[0-9]+]], sp
; CHECK: movk [[MOD1]], #{{[0-9]+}}, lsl #48
; CHECK: pacga [[CAN1:x[0-9]+]], [[MOD1]], {{x[0-9]+}}
; CHECK: {{str|stp x30,}} [[CAN1]]
; CHECK: bl expose
; // Make sure the canary is loaded!
; CHECK: {{ldr|ldp .*}} [[CAN2:x[0-9]+]]
; // Make sure the reference canary is re-created from scratch!
; CHECK: mov [[MOD2:x[0-9]+]], sp
; CHECK: movk [[MOD2]], #{{[0-9]+}}, lsl #48
; CHECK: pacga [[REF_CAN:x[0-9]+]], [[MOD2]], {{x[0-9]+}}
; // Finally, just check the canary...
; CHECK: cmp [[CAN2]], [[REF_CAN]]
; CHECK: ret
define hidden void @simple() {
entry:
  %a = alloca [32 x i8]
  %arraydecay1 = getelementptr inbounds [32 x i8], [32 x i8]* %a, i32 0, i32 0
  call void @expose(i8* %arraydecay1)
  ret void
}

declare hidden void @expose(i8* %str)
