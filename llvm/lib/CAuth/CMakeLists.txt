#===----------------------------------------------------------------------===//
#
# Author: Zaheer Gauhar <zaheer.gauhar@pm.me>
#         Hans Liljestrand <hans@liljestrand.dev>
# ***Copyright: Secure Systems Group, Aalto University https://ssg.aalto.fi/***
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
#===----------------------------------------------------------------------===//

add_llvm_library(Cauth
  CAuth.cpp
  CAuthIntr.cpp
  CAuthCanaryPass.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/CAUTH
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Support
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/IR

  DEPENDS
  LLVMCore
  LLVMSupport
  LLVMCodeGen

  LINK_LIBS
  LLVMCore
  LLVMSupport
  LLVMCodeGen
)

set_property(TARGET Cauth PROPERTY LLVM_SYSTEM_LIBS)
