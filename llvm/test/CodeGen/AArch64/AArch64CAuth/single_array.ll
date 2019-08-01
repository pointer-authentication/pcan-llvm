;
; RUN: llc -verify-machineinstrs -mtriple=aarch64-none-linux-gnu -mattr=v8.3a -cauth=arr < %s | FileCheck %s

; CHECK-LABEL: @simple
define hidden void @simple() {
entry:
; // Make sure we and store the canary
; CHECK mov	[[MOD1:x[0-9]+]], sp
; CHECK movk [[MOD1]], #{{[0-9]+}}, lsl #48
; CHECK pacga x19, xzr, [[MOD1]]
; CHECK str x19, [[CAN_ADDR:.*$]]
  %a = alloca [32 x i8]
  %arraydecay1 = getelementptr inbounds [32 x i8], [32 x i8]* %a, i32 0, i32 0
; CHECK bl printer
  call void @printer(i8* %arraydecay1)
  ret void
; // Make sure the canary is checked!
; CHECK ldr [[CAN:x[0-9]+]], [[CAN_ADDR]]
; CHECK cmp [[CAN]],
; CHECK ret
}

declare hidden void @printer(i8* %str)
