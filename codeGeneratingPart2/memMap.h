//
// Created by SAMSUNG on 2020/11/13.
//

#ifndef CODEGENERATING_MEMMAP_H
#define CODEGENERATING_MEMMAP_H

#include <unordered_map>
#include "quadruple.h"
#include "config.h"
#include "symbolTable.h"

using namespace std;

typedef struct Con1 {
    string name;
    string address;
    quaItem *nameTemplate;
}addressCon;

extern vector<addressCon> globalMapping;
extern vector<addressCon> functionMapping;

string assignSpace(string name, int bytes, bool isGlobalOrMain, quaItem *thisTemplate = nullptr);
string getAddress(quadruple *ptr, int which, bool isGlobalOrMain);
int get_num_address(quadruple *ptr, int which = 0);
string getBaseAddressByName(string name);
void printFunctionMapping();
void printGlobalMapping();
void functionEnds();
quaItem* getTemplate(string name);
int found_in_function_map(string name);

int parseAddress(string str);

extern int stackTop;
extern int globalTop;

#endif //CODEGENERATING_MEMMAP_H
