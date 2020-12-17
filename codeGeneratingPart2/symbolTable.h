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
#include "error.h"
#include "myVectors.h"
#include "quadruple.h"

using namespace std;

typedef struct tableItem {
    string codeName;
    Type type;
    int space;
    bool isConst;
    bool isFunc;
    int level;
    int dimension;
    quaItem *thisTemplate;
    bool operator==(const tableItem b) const
    {
        return this->codeName == b.codeName;
    }
}tableItem, *tablePt;

extern vector<tableItem> table;
extern tableItem insertItem;
extern tableItem searchItem;
extern tableItem unfoundItem;
extern vector<int> spaceNeeded;
extern vector<int> paraCntVector;
extern vector<Type> returnTypeVector;
extern vector<string> functionNameVector;

void setItem(tablePt pt, string name, Type type, int space, bool isConst, bool isFunc, int level, int dimension, quaItem *thisTemplate = nullptr);
tablePt find(tableItem target);
void printItem(tableItem item);
Type getTemplate(tablePt pt, int *row_num, int *col_num, int *unit_bytes);

int registerItem(string name, Type type, int space, bool isConst, bool isFunc, int thisLevel, int dimension, quaItem *thisTemplate = nullptr);
void popTable(int now_level);

int get_space_by_func_name(string funcName);


#endif //SPLITERRORDEALING_SYMBOLTABLE_H
