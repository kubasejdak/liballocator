#!/bin/bash
# $1 - CMake version to be installed (e.g. 3.8.2)
# $2 - Host OS (Linux or macOS)

VERSION=${1}
if [ "${2}" == "Linux" ]; then
    OS="Linux"
elif [ "${2}" == "macOS" ]; then
    OS="Darwin"
fi

if [ -z ${VERSION} ]; then
    echo "No CMake version specified. Aborting."
    exit 1
fi

if [ -z ${OS} ]; then
    echo "No host OS specified. Aborting."
    exit 2
fi

echo "Installing CMake v${VERSION}"

SHORT_VERSION=`echo ${VERSION} | cut -d . -f 1-2`
PACKAGE_NAME="cmake-${VERSION}-${OS}-x86_64"
PACKAGE_BIN_NAME="${PACKAGE_NAME}.tar.gz"
PACKAGE_URL="https://cmake.org/files/v${SHORT_VERSION}/${PACKAGE_BIN_NAME}"

wget --no-check-certificate ${PACKAGE_URL}
tar -xzf ${PACKAGE_BIN_NAME}

export PATH=${PWD}/${PACKAGE_NAME}/bin:${PATH}

echo "Installing CMake v${VERSION} OK."