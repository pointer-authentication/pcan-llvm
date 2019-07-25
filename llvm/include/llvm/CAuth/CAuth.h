//===----------------------------------------------------------------------===//
//
// Author: Zaheer Gauhar <zaheer.gauhar@pm.me>
// *******Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/**********
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CAUTH_H
#define LLVM_CAUTH_H

namespace llvm {

namespace CAuth {

bool useCAuth();
bool useCAuthMod();
bool useTest();
bool useDummy();

} // CAuth

} // llvm

#endif //LLVM_CAUTH_H
