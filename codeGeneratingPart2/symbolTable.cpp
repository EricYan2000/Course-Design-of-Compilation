//
// Created by SAMSUNG on 2020/11/10.
//

#include "symbolTable.h"

tableItem insertItem = {
        .codeName = "",
        .type = undefinedType,
        .space = 4,
        .isConst = false,
        .isFunc = false,
        .level = -1,
        .dimension = 0,
        .thisTemplate = nullptr,
};
tableItem searchItem = {
        .codeName = "",
        .type = undefinedType,
        .space = 4,
        .isConst = false,
        .isFunc = false,
        .level = -1,
        .dimension = 0,
        .thisTemplate = nullptr,
};
tableItem unfoundItem = {
        .codeName = "",
        .type = undefinedType,
        .space = 0,
        .isConst = false,
        .isFunc = false,
        .level = -1,
        .dimension = 0,
        .thisTemplate = nullptr,
};

vector<tableItem> table;
vector<int> spaceNeeded;
vector<int> paraCntVector;
vector<Type> returnTypeVector;
vector<string> functionNameVector;

void setItem(tablePt pt, string name, Type type, int space, bool isConst, bool isFunc, int level, int dimension, quaItem *thisTemplate) {
    string str = name;
    transform(str.begin(), str.end(), str.begin(), ::tolower); //still be needed
    pt->codeName = str;
    pt->type = type;
    pt->space = space;
    pt->isConst = isConst;
    pt->isFunc = isFunc;
    pt->level = level;
    pt->dimension = dimension;
    pt->thisTemplate = thisTemplate;
}

tablePt find(tableItem target) {
    for (int i = table.size() - 1; i >= 0; i--) {
        if (table[i] == target)
            return &table[i];
    }
    return &unfoundItem;
}

Type getTemplate(tablePt pt, int *row_num, int *col_num, int *unit_bytes) {
    *row_num = pt->thisTemplate->getRowNum();
    *col_num = pt->thisTemplate->getColNum();
    *unit_bytes = pt->thisTemplate->getUnitBytes();
    return pt->thisTemplate->getType();
}

int registerItem(string name, Type type, int space, bool isConst, bool isFunc, int thisLevel, int dimension, quaItem *thisTemplate) {
    setItem(&insertItem, name, type, space, isConst, isFunc, thisLevel, dimension, thisTemplate);
    tablePt found = find(insertItem);
    if (found == NULL || found->level < thisLevel) {
        table.push_back(insertItem);
        if (tableDebug) {
            printf("[registered] ");
            printItem(insertItem);
        }
        return 1;
    } else {
        error("b", peekLine(-1));
        return 0;
    }
}

void popTable(int now_level) {
    bool flag = true;
    int spaceCnt = 0;
    while (table.size() != 0 && flag) {
        if (table[table.size() - 1].level > now_level) {
            spaceCnt += table.back().space;
            if (tableDebug) {
                tableItem toBePoped = table[table.size() - 1];
                printf("[poped] ");
                printItem(toBePoped);
            }
            table.pop_back();
        }
        else
            flag = false;
    }
    if (now_level == 0)
        spaceNeeded.push_back(spaceCnt + 8);
}

void printItem(tableItem item) {
    string codeName = item.codeName;
    string type = "";
    if (item.type == undefinedType)
        type = "undefinedType";
    else if (item.type == intType)
        type = "intType";
    else if (item.type == charType)
        type = "charType";
    else if (item.type == voidType)
        type = "voidType";
    else
        type = "funcType";
    string isConst = (item.isConst) ? "isConst" : "notConst";
    string isFunc = (item.isFunc) ? "isFunc" : "notFunc";
    printf("%s %s %d %s %s level:%d dimension:%d\n", codeName.c_str(), type.c_str(), item.space
            , isConst.c_str(), isFunc.c_str(), item.level, item.dimension);
}

int get_space_by_func_name(string funcName) {
    for (int i = 0; i < functionNameVector.size(); i++) {
        if (functionNameVector[i] == funcName)
            return spaceNeeded[i];
    }
}



