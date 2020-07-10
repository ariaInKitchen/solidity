#!/usr/bin/env bash
set -e

export ROOTDIR="$(dirname "$0")/.."
export BUILDDIR="${ROOTDIR}/build"

if [[ $# -eq 0 ]]; then
    BUILD_TYPE=Release
else
    BUILD_TYPE="$1"
fi

if [[ "$(git tag --points-at HEAD 2>/dev/null)" == v* ]]; then
	touch "${ROOTDIR}/prerelease.txt"
fi

mkdir -p "${BUILDDIR}"

"${ROOTDIR}/scripts/build-gmssl.sh" $params;

cd "${BUILDDIR}"

cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE" "${@:2}"
make -j2

if [[ "${CI}" == "" ]]; then
	echo "Installing ..."
	sudo make install
fi
