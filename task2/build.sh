#!/bin/bash

echo "----------COMPILING TESTCASES----------"
clang -g -S -emit-llvm -c escape.c escape2.c escape3.c
echo -e "\n"

echo "----------COMPILING PROGRAM----------"
clang++ -std=c++11 -o asg2-task2 asg2-task2.cpp `llvm-config --cxxflags` `llvm-config --ldflags` `llvm-config --libs` -lpthread -lncurses -ldl
echo -e "\n"

echo "----------RUNNING TEST 1----------"
./asg2-task2 escape.ll
echo -e "\n"

echo "----------RUNNING TEST 2----------"
./asg2-task2 escape2.ll 
echo -e "\n"

echo "----------RUNNING TEST 3----------"
./asg2-task2 escape3.ll 
echo -e "\n"
