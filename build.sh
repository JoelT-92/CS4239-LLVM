#!/bin/bash

clang -g -S -emit-llvm -c dead.c dead2.c dead2.h

clang++ -std=c++11 -o asg2-task1 asg2-task1.cpp `llvm-config --cxxflags` `llvm-config --ldflags` `llvm-config --libs` -lpthread -lncurses -ldl

./asg2-task1 dead.ll dead2.ll 
