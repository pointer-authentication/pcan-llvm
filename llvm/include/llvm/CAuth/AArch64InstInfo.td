//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
//         Hans Liljestrand <hans@liljestrand.dev>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//


let isPseudo = 1, hasNoSchedulingInfo = 1 in {

  def CAUTH_PRO_MOD : Pseudo<(outs GPR64:$dst), (ins),
                              [(set i64:$dst, (int_cauth_pro_mod))]>;

  def CAUTH_EPI_MOD : Pseudo<(outs GPR64:$dst), (ins),
                              [(set i64:$dst, (int_cauth_epi_mod))]>;

  def CAUTH_PACGA : Pseudo<(outs GPR64:$dst),
                           (ins GPR64:$src),
                           [(set i64:$dst, (int_ca_pacga GPR64:$src))]>;

  def CAUTH_PACDA : Pseudo<(outs GPR64:$dst),
                           (ins GPR64:$ptr, GPR64:$mod),
                           [(set i64:$dst, (int_ca_pacda
                                            GPR64:$ptr,
                                            GPR64:$mod))],
                           "$dst = $ptr">;

  def CAUTH_AUTDA : Pseudo<(outs GPR64:$dst),
                           (ins GPR64:$ptr, GPR64:$mod),
                           [(set i64:$dst, (int_ca_autda
                                            GPR64:$ptr,
                                            GPR64:$mod))],
                           "$dst = $ptr">;
}

