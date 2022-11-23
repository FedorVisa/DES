cmake_minimum_required(VERSION 3.23)
project(DES)

set(CMAKE_CXX_STANDARD 20)

add_executable(DES main.cpp DES.h Des.cpp des_key.h des_key.h des_tables.h )
