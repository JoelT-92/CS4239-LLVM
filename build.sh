#!/bin/bash

clang -g -S -emit-llvm -c dead.c

clang++ -std=c++11 -o asg2-task1 asg2-task1.cpp `llvm-config --cxxflags` `llvm-config --ldflags` `llvm-config --libs` -lpthread -lncurses -ldl

./asg2-task1 dead.ll
