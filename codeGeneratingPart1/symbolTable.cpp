//
// Created by SAMSUNG on 2020/11/10.
//

#include "symbolTable.h"

tableItem insertItem = {
        .codeName = "",
        .type = undefinedType,
        .isConst = false,
        .isFunc = false,
        .level = -1,
        .dimension = 0,
};
tableItem searchItem = {
        .codeName = "",
        .type = undefinedType,
        .isConst = false,
        .isFunc = false,
        .level = -1,
        .dimension = 0,
};

vector<tableItem> table;

void setItem(tablePt pt, string name, Type type, bool isConst, bool isFunc, int level, int dimension) {
    string str = name;
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    pt->codeName = str;
    pt->type = type;
    pt->isConst = isConst;
    pt->isFunc = isFunc;
    pt->level = level;
    pt->dimension = dimension;
}

tablePt find(tableItem target) {
    tablePt pt = NULL;
    for (int i = table.size() - 1; i >= 0; i--) {
        if (table[i] == target)
            return &table[i];
    }
    return NULL;
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
    printf("%s %s %s %s level:%d dimension:%d\n", codeName.c_str(), type.c_str()
            , isConst.c_str(), isFunc.c_str(), item.level, item.dimension);
}



