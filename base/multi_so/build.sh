#!/usr/bin/env bash
echo "build start *************************"

ndk-build NDK_APPLICATION_MK=Application.mk NDK_PROJECT_PATH=. clean

ndk-build NDK_APPLICATION_MK=Application.mk NDK_PROJECT_PATH=.

echo "build end  **************************"
