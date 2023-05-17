#!/bin/bash
g++ procedure2_no_multithread.cpp -O3 -std=c++20 -o main
g++ procedure2.cpp -O3 -std=c++20 -o main_threaded
g++ procedure2_test.cpp -O3 -std=c++20 -o main_test
