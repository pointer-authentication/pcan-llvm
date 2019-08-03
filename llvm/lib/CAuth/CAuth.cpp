//===----------------------------------------------------------------------===//
//
// Author: Zaheer Gauhar <zaheer.gauhar@pm.me>
// *******Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/**********
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm/CAuth/CAuth.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace llvm::CAuth;

static cl::opt<bool> UseDummyInstructions("cauth-dummy", cl::Hidden,
                                          cl::desc("Use dummy instructions and XOR instead of PA"),
                                          cl::init(false));

static cl::opt<CAuthType> CAuthTypeOpt(
    "cauth", cl::init(CAuthNone),
    cl::desc("CAuth mode to use"),
    cl::value_desc("mode"),
    cl::values(clEnumValN(CAuthNone, "none", "Disable CAuth"),
               clEnumValN(CAuthArrays, "arr", "Protect all stack buffers")));

static cl::opt<bool> CAuthTargetOnly("cauth-noir", cl::Hidden,
                                     cl::desc("Do not run the CAuth IR passes in Target"),
                                     cl::init(false));

bool llvm::CAuth::runCAuthCanaryPassInTarget() {
  return CAuthTypeOpt != CAuthNone && !CAuthTargetOnly;
}

bool llvm::CAuth::useCAuth() {
  return CAuthTypeOpt != CAuthNone;
}

bool llvm::CAuth::useDummy() {
  return UseDummyInstructions;
}
