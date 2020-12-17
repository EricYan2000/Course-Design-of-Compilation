cmake_minimum_required(VERSION 3.17)
project(speedRacing)

set(CMAKE_CXX_STANDARD 14)

add_executable(speedRacing main.cpp wordDealing.h wordDealing.cpp config.h config.cpp
        symbolTable.h symbolTable.cpp type.h quadruple.h quadruple.cpp memMap.h memMap.cpp
        error.h error.cpp myVectors.h myVectors.cpp)