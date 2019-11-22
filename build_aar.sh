#!/usr/bin/env bash
echo "build aar start *************************"

./gradlew :ffmpeglibrary:assembleRelease -x lint

cp -rf ffmpeglibrary/build/outputs/aar/ffmpeglibrary-release.aar app/libs/

echo "build aar end  **************************"
