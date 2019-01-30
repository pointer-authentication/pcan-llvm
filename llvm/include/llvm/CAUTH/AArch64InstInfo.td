//===----------------------------------------------------------------------===//
//
// Author: Zaheer Ahmed Gauhar <zaheer.gauhar@pm.me>
// ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

let isPseudo = 1 in {
  def CAUTH_PACGA : Pseudo<(outs GPR64:$dst),
                           (ins GPR64:$src, GPR64:$mod),
                           [(set i64:$dst, (int_ca_pacga GPR64:$src, GPR64:$mod))]>,
                    Sched<[]>;
}

let isPseudo = 1 in {
  def CAUTH_PACDA : Pseudo<(outs GPR64:$dst),
                           (ins GPR64:$ptr, GPR64:$mod),
                           [(set i64:$dst, (int_ca_pacda GPR64:$ptr, GPR64:$mod))]>,
                    Sched<[]>;
}

let isPseudo = 1 in {
  def CAUTH_AUTDA : Pseudo<(outs GPR64:$dst),
                           (ins GPR64:$ptr, GPR64:$mod),
                           [(set i64:$dst, (int_ca_autda GPR64:$ptr, GPR64:$mod))]>,
                    Sched<[]>;
}

