//
// Created by SAMSUNG on 2020/11/10.
//

#ifndef SPLITERRORDEALING_SYMBOLTABLE_H
#define SPLITERRORDEALING_SYMBOLTABLE_H

#include <iostream>
#include <vector>
#include <algorithm>
#include "type.h"
#include "config.h"

using namespace std;

typedef struct tableItem {
    string codeName;
    Type type;
    bool isConst;
    bool isFunc;
    int level;
    int dimension;
    bool operator==(const tableItem b) const
    {
        return this->codeName == b.codeName;
    }
}tableItem, *tablePt;

extern vector<tableItem> table;
extern tableItem insertItem;
extern tableItem searchItem;

void setItem(tablePt pt, string name, Type type, bool isConst, bool isFunc, int level, int dimension);
tablePt find(tableItem target);
void printItem(tableItem item);


#endif //SPLITERRORDEALING_SYMBOLTABLE_H
