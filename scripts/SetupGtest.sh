#! /bin/bash

# assumes this is executed from the build dir
cd ..
if [ ! -d gtest-1.7.0 ]; then
    wget http://googletest.googlecode.com/files/gtest-1.7.0.zip
    unzip gtest-1.7.0.zip
    rm gtest-1.7.0.zip
fi
cd build
