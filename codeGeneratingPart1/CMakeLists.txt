cmake_minimum_required(VERSION 3.17)
project(codeGenerating)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXE_LINKER_FLAGS -static)

add_executable(codeGenerating main.cpp wordDealing.h wordDealing.cpp config.h config.cpp symbolTable.h symbolTable.cpp type.h quadruple.h quadruple.cpp memMap.h memMap.cpp)