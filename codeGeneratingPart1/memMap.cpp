//
// Created by SAMSUNG on 2020/11/13.
//

#include "memMap.h"

int stackTop = 0x10040000;
unordered_map<string, string> memMapping;

string assignSpace(quadruple *ptr) {
    stackTop = stackTop - 4;
    memMapping[ptr->getName()] = to_string(stackTop);
    return to_string(stackTop);
}

string getAddress(quadruple *ptr, int which) {
    string name = "";
    if (which == 0) {
        name = ptr->getName();
    } else if (which == 1) {
        name = ptr->getItem1()->getName();
    } else if (which == 2) {
        name = ptr->getItem2()->getName();
    } else if (which == 3) {
        name = ptr->getItem3()->getName();
    } else {
        printf("in getAddress, which no in range\n");
    }
    int cnt = memMapping.count(name);
    if (cnt != 0)
        return memMapping[name];
    else
        return assignSpace(ptr);
}

void printMapping() {
    if (!mappingDebug)
        return ;
    for (auto i = memMapping.begin(); i != memMapping.end(); i++) {
        string name = i->first;
        string address = i->second;
        printf("[memMapping] %s\t in\t %s\n", name.c_str(), address.c_str());
    }
}

