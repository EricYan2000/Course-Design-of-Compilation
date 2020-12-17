//
// Created by SAMSUNG on 2020/12/8.
//

#include "error.h"

void error(string errorCode, int line, string info) { //default missing 没有用这个函数
    int length = errorCode.length();
    if (length == 1) {
        fprintf(errorOUT, "%d %s\n", line, errorCode.c_str());
    } else {
        printf("%s\n", errorCode.c_str());
    }
}

