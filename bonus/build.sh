#!/bin/bash

echo "----------COMPILING TESTCASES----------"
clang -g -S -emit-llvm -c globalescape.c
echo -e "\n"

echo "----------COMPILING PROGRAM----------"
clang++ -std=c++11 -o asg2-bonus asg2-bonus.cpp `llvm-config --cxxflags` `llvm-config --ldflags` `llvm-config --libs` -lpthread -lncurses -ldl
echo -e "\n"

echo "----------RUNNING TEST 1----------"
./asg2-bonus globalescape.ll
echo -e "\n"
