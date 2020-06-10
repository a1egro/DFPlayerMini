cmake_minimum_required(VERSION 3.10)
project(DFPlayerMini)

set(CMAKE_CXX_STANDARD 11)

add_library(DFPlayerMini dfplayer.cpp dfplayer.h serial.cpp serial.h callback.cpp callback.h)