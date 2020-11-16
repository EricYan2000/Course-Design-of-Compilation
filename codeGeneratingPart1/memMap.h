//
// Created by SAMSUNG on 2020/11/13.
//

#ifndef CODEGENERATING_MEMMAP_H
#define CODEGENERATING_MEMMAP_H

#include <unordered_map>
#include "quadruple.h"
#include "config.h"

using namespace std;

extern unordered_map<string, string> memMapping;

string assignSpace(quadruple *ptr);
string getAddress(quadruple *ptr, int which = 0);
void printMapping();

extern int stackTop;

#endif //CODEGENERATING_MEMMAP_H
