//
// Created by SAMSUNG on 2020/12/8.
//

#ifndef CODEGENERATING_ERROR_H
#define CODEGENERATING_ERROR_H

#include <unordered_map>
#include <string.h>
#include <algorithm>
#include <stdio.h>
#include "config.h"

using namespace std;

void error(string errorCode = "", int line = 0, string info = "");

#endif //CODEGENERATING_ERROR_H
