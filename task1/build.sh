#!/bin/bash

echo "----------COMPILING TESTCASES----------"
clang -g -S -emit-llvm -c dead.c dead2.c dead3.c dead4.c dead5.c
echo -e "\n"

echo "----------COMPILING PROGRAM----------"
clang++ -std=c++11 -o asg2-task1 asg2-task1.cpp `llvm-config --cxxflags` `llvm-config --ldflags` `llvm-config --libs` -lpthread -lncurses -ldl
echo -e "\n"

echo "----------RUNNING TEST 1----------"
./asg2-task1 dead.ll
echo -e "\n"

echo "----------RUNNING TEST 2----------"
./asg2-task1 dead2.ll 
echo -e "\n"

echo "----------RUNNING TEST 3----------"
./asg2-task1 dead.ll dead2.ll 
echo -e "\n"

echo "----------RUNNING TEST 4----------"
./asg2-task1 dead3.ll
echo -e "\n"

echo "----------RUNNING TEST 5----------"
./asg2-task1 dead4.ll
echo -e "\n"

echo "----------RUNNING TEST 6----------"
./asg2-task1 dead5.ll
echo -e "\n"