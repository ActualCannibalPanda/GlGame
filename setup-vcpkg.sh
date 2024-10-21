#!/usr/bin/env bash

./external/vcpkg/bootstrap-vcpkg.sh -disableMetrics
export VCPKG_ROOT=$(pwd)/external/vcpkg
export PATH=$VCPKG_ROOT:$PATH

vcpkg install --triplet x64-linux
