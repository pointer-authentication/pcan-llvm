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
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;

static cl::opt<bool> EnableCAuth("cauth", cl::Hidden,
                                      cl::desc("Canary Authentication"),
                                      cl::init(false));


static cl::opt<bool> UseDummyInstructions("cauth-dummy", cl::Hidden,
                                          cl::desc("Use dummy instructions and XOR instead of PA"),
                                          cl::init(false));

static cl::opt<bool> EnableTest("test", cl::Hidden,
                                      cl::desc("Test pass for MIR"),
                                      cl::init(false));


bool llvm::CAuth::useCAuth() {
  return EnableCAuth;
}


bool llvm::CAuth::useDummy() {
  return UseDummyInstructions;
}

bool llvm::CAuth::useTest() {
  return EnableTest;
}
