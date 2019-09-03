; RUN: opt -O2 -cauth=arr -cauth-ir-arrays -S < %s | FileCheck %s
;
; Caused the following error:
;
; Instruction does not dominate all uses!
;   %13 = call i64 @llvm.cauth.epi.mod()
;   %ega2 = call i64 @llvm.ca.pacga(i64 %13)
; in function lzma_index_hash_append
; LLVM ERROR: Broken function found, compilation aborted!

%struct.lzma_check_state = type { %union.anon.25, %union.anon.0 }
%union.anon.25 = type { [8 x i64] }
%union.anon.0 = type { %struct.anon.26 }
%struct.anon.26 = type { [8 x i32], i64 }
%struct.lzma_index_hash_s = type { i32, %struct.lzma_index_hash_info, %struct.lzma_index_hash_info, i64, i64, i64, i64, i32 }
%struct.lzma_index_hash_info = type { i64, i64, i64, i64, %struct.lzma_check_state }

; CHECK-LABEL: @lzma_index_hash_append
; CHECK: entry
define dso_local i32 @lzma_index_hash_append(%struct.lzma_index_hash_s* %index_hash, i64 %unpadded_size, i64 %uncompressed_size) local_unnamed_addr #0 {
entry:
  %sizes.i = alloca [2 x i64], align 8
  %sequence = getelementptr inbounds %struct.lzma_index_hash_s, %struct.lzma_index_hash_s* %index_hash, i64 0, i32 0
  %0 = load i32, i32* %sequence, align 8, !tbaa !232
  %cmp = icmp ne i32 %0, 0
  %unpadded_size.off = add i64 %unpadded_size, -5
  %1 = icmp ugt i64 %unpadded_size.off, 9223372036854775799
  %2 = or i1 %1, %cmp
  %cmp5 = icmp slt i64 %uncompressed_size, 0
  %or.cond32 = or i1 %cmp5, %2
  br i1 %or.cond32, label %return, label %do.body

do.body:                                          ; preds = %entry
  %add.i.i = add i64 %unpadded_size, 3
  %and.i.i = and i64 %add.i.i, -4
  %blocks_size.i = getelementptr inbounds %struct.lzma_index_hash_s, %struct.lzma_index_hash_s* %index_hash, i64 0, i32 1, i32 0
  %3 = load i64, i64* %blocks_size.i, align 8, !tbaa !237
  %add.i = add i64 %3, %and.i.i
  store i64 %add.i, i64* %blocks_size.i, align 8, !tbaa !237
  %uncompressed_size1.i = getelementptr inbounds %struct.lzma_index_hash_s, %struct.lzma_index_hash_s* %index_hash, i64 0, i32 1, i32 1
  %4 = load i64, i64* %uncompressed_size1.i, align 8, !tbaa !238
  %add2.i = add i64 %4, %uncompressed_size
  store i64 %add2.i, i64* %uncompressed_size1.i, align 8, !tbaa !238
  %call3.i = tail call i32 @lzma_vli_size(i64 %unpadded_size) #4
  %call4.i = tail call i32 @lzma_vli_size(i64 %uncompressed_size) #4
  %add5.i = add i32 %call4.i, %call3.i
  %conv.i = zext i32 %add5.i to i64
  %index_list_size.i = getelementptr inbounds %struct.lzma_index_hash_s, %struct.lzma_index_hash_s* %index_hash, i64 0, i32 1, i32 3
  %5 = load i64, i64* %index_list_size.i, align 8, !tbaa !239
  %add6.i = add i64 %5, %conv.i
  store i64 %add6.i, i64* %index_list_size.i, align 8, !tbaa !239
  %count.i = getelementptr inbounds %struct.lzma_index_hash_s, %struct.lzma_index_hash_s* %index_hash, i64 0, i32 1, i32 2
  %6 = load i64, i64* %count.i, align 8, !tbaa !240
  %inc.i = add i64 %6, 1
  store i64 %inc.i, i64* %count.i, align 8, !tbaa !240
  %7 = bitcast [2 x i64]* %sizes.i to i8*
  call void @llvm.lifetime.start.p0i8(i64 16, i8* nonnull %7) #4
  %arrayinit.begin.i = getelementptr inbounds [2 x i64], [2 x i64]* %sizes.i, i64 0, i64 0
  store i64 %unpadded_size, i64* %arrayinit.begin.i, align 8, !tbaa !26
  %arrayinit.element.i = getelementptr inbounds [2 x i64], [2 x i64]* %sizes.i, i64 0, i64 1
  store i64 %uncompressed_size, i64* %arrayinit.element.i, align 8, !tbaa !26
  %check.i = getelementptr inbounds %struct.lzma_index_hash_s, %struct.lzma_index_hash_s* %index_hash, i64 0, i32 1, i32 4
  call void @lzma_check_update(%struct.lzma_check_state* nonnull %check.i, i32 10, i8* nonnull %7, i64 16) #4
  call void @llvm.lifetime.end.p0i8(i64 16, i8* nonnull %7) #4
  %8 = load i64, i64* %blocks_size.i, align 8, !tbaa !241
  %cmp10 = icmp slt i64 %8, 0
  br i1 %cmp10, label %return, label %lor.lhs.false11

lor.lhs.false11:                                  ; preds = %do.body
  %9 = load i64, i64* %uncompressed_size1.i, align 8, !tbaa !242
  %cmp14 = icmp slt i64 %9, 0
  br i1 %cmp14, label %return, label %lor.lhs.false15

lor.lhs.false15:                                  ; preds = %lor.lhs.false11
  %10 = load i64, i64* %count.i, align 8, !tbaa !235
  %11 = load i64, i64* %index_list_size.i, align 8, !tbaa !236
  %call.i.i = call i32 @lzma_vli_size(i64 %10) #4
  %add.i.i46 = add i32 %call.i.i, 1
  %conv.i.i = zext i32 %add.i.i46 to i64
  %add2.i.i = add i64 %11, 7
  %add.i2.i = add i64 %add2.i.i, %conv.i.i
  %and.i.i47 = and i64 %add.i2.i, -4
  %cmp19 = icmp ugt i64 %and.i.i47, 17179869184
  br i1 %cmp19, label %return, label %lor.lhs.false20

lor.lhs.false20:                                  ; preds = %lor.lhs.false15
  %12 = load i64, i64* %blocks_size.i, align 8, !tbaa !241
  %13 = load i64, i64* %count.i, align 8, !tbaa !235
  %14 = load i64, i64* %index_list_size.i, align 8, !tbaa !236
  %call.i.i.i = call i32 @lzma_vli_size(i64 %13) #4
  %add.i.i.i = add i32 %call.i.i.i, 1
  %conv.i.i.i = zext i32 %add.i.i.i to i64
  %add2.i.i.i = add i64 %14, 7
  %add.i2.i.i = add i64 %add2.i.i.i, %conv.i.i.i
  %and.i.i.i = and i64 %add.i2.i.i, -4
  %add1.i = add i64 %12, 24
  %add2.i48 = add i64 %add1.i, %and.i.i.i
  %cmp28 = icmp slt i64 %add2.i48, 0
  %spec.select = select i1 %cmp28, i32 9, i32 0
  ret i32 %spec.select

return:                                           ; preds = %do.body, %lor.lhs.false11, %lor.lhs.false15, %entry
  %retval.1 = phi i32 [ 11, %entry ], [ 9, %lor.lhs.false15 ], [ 9, %lor.lhs.false11 ], [ 9, %do.body ]
  ret i32 %retval.1
}

declare dso_local i32 @lzma_vli_size(i64 %vli) #7
declare dso_local void @lzma_check_update(%struct.lzma_check_state* %check, i32 %type, i8* %buf, i64 %size) #0

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.start.p0i8(i64, i8* nocapture) #5

; Function Attrs: argmemonly nounwind
declare void @llvm.lifetime.end.p0i8(i64, i8* nocapture) #5

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #5 = { argmemonly nounwind }
attributes #7 = { norecurse nounwind readnone "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="generic" "target-features"="+neon,+v8.3a" "unsafe-fp-math"="false" "use-soft-float"="false" }

!4 = !{!"omnipotent char", !5, i64 0}
!5 = !{!"Simple C/C++ TBAA"}
!8 = !{!"long", !4, i64 0}
!9 = !{!"int", !4, i64 0}
!26 = !{!8, !8, i64 0}
!155 = !{!"", !4, i64 0, !4, i64 64}
!232 = !{!233, !4, i64 0}
!233 = !{!"lzma_index_hash_s", !4, i64 0, !234, i64 8, !234, i64 144, !8, i64 280, !8, i64 288, !8, i64 296, !8, i64 304, !9, i64 312}
!234 = !{!"", !8, i64 0, !8, i64 8, !8, i64 16, !8, i64 24, !155, i64 32}
!235 = !{!233, !8, i64 24}
!236 = !{!233, !8, i64 32}
!237 = !{!234, !8, i64 0}
!238 = !{!234, !8, i64 8}
!239 = !{!234, !8, i64 24}
!240 = !{!234, !8, i64 16}
!241 = !{!233, !8, i64 8}
!242 = !{!233, !8, i64 16}
