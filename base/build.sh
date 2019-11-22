#!/usr/bin/env bash
echo "build start *************************"

ndk-build NDK_APPLICATION_MK=Application.mk NDK_PROJECT_PATH=. clean

ndk-build NDK_APPLICATION_MK=Application.mk NDK_PROJECT_PATH=.

cp -rf libs/armeabi-v7a/libffplayer.so ../ffmpeglibrary/libs/armeabi-v7a/

cd ../

./build_aar.sh

cd base

echo "build end  **************************"
