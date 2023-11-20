#!/bin/bash

mkdir -p objs && mkdir -p objs/src
clang++ -std=c++17 -c main.cpp -o objs/main.o
clang++ -std=c++17 -c src/trace.cpp -o objs/src/trace.o
clang++ objs/main.o objs/src/trace.o -o app
./app