#!/bin/bash
#
# Author: Hans Liljestrand <hans@liljestrand.dev>
# Copyright: Secure Systems Group, Aalto University, https://ssg.aalto.fi
#
# This code is released under Apache 2.0 license

set -ue

SCRIPT_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

LINARO_RELEASE_URL="https://releases.linaro.org/components/toolchain/binaries/7.3-2018.05/aarch64-linux-gnu/"
LINARO_SYSROOT_FILENAME="sysroot-glibc-linaro-2.25-2018.05-aarch64-linux-gnu.tar.xz"
LINARO_SYSROOT_URL="${LINARO_RELEASE_URL}/${LINARO_SYSROOT_FILENAME}"
LINARO_GCC_FILENAME="gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu.tar.xz"
LINARO_GCC_URL="${LINARO_RELEASE_URL}/${LINARO_GCC_FILENAME}"

install_sysroot() {
    cd "${SCRIPT_ROOT}"
    if [[ -e sysroot ]]; then
        echo "$(pwd)/sysroot exists, doing nothing...."
    else
        echo "Setting up Linaro sysroot at $(pwd)/sysroot ..."
        wget "${LINARO_SYSROOT_URL}"
        tar xJf "${LINARO_SYSROOT_FILENAME}"
        mv sysroot-glibc-linaro-2.25-2018.05-aarch64-linux-gnu sysroot
        rm "${LINARO_SYSROOT_FILENAME}"
    fi
}

install_gcc() {
    cd "${SCRIPT_ROOT}"
    if [[ -e gcc ]]; then
        echo "$(pwd)/gcc exists, doing nothing...."
    else
        echo "Setting up Linaro GCC at $(pwd)/gcc ..."
        wget "${LINARO_GCC_URL}"
        tar xJf "${LINARO_GCC_FILENAME}"
        mv gcc-linaro-7.3.1-2018.05-x86_64_aarch64-linux-gnu gcc
        rm "${LINARO_GCC_FILENAME}"
    fi
}

install_sysroot
install_gcc

