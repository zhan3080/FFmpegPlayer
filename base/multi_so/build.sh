#!/usr/bin/env bash
echo "build start *************************"

ndk-build NDK_APPLICATION_MK=Application.mk NDK_PROJECT_PATH=. clean

ndk-build NDK_APPLICATION_MK=Application.mk NDK_PROJECT_PATH=.

rm -rf ../../ffmpeglibrary/libs/armeabi-v7a/*

cp -rf libs/armeabi-v7a/*.so ../../ffmpeglibrary/libs/armeabi-v7a/

echo "build end  **************************"
