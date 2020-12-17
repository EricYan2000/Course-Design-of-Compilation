//
// Created by SAMSUNG on 2020/11/13.
//

#include "memMap.h"

int stackTop = 0x7fffeffc;
int globalTop = 0x10010000;
int functionConsumed = 8;

vector<addressCon> globalMapping;
vector<addressCon> functionMapping;

string assignSpace(string name, int bytes, bool isGlobalOrMain, quaItem *thisTemplate) {
    if (isGlobalOrMain) {
        addressCon thisCon = addressCon{
            .name = name,
            .address = to_string(globalTop),
            .nameTemplate = thisTemplate,
        };
        globalMapping.push_back(thisCon);
        globalTop += bytes;
        return thisCon.address;
    } else {
        addressCon thisCon = addressCon{
                .name = name,
                .address = to_string(functionConsumed),
                .nameTemplate = thisTemplate,
        };
        functionMapping.push_back(thisCon);
        functionConsumed += bytes;
        return thisCon.address;
    }
}

void functionEnds() {
    printFunctionMapping();
    functionConsumed = 8;
    functionMapping.clear();
}

string getAddress(quadruple *ptr, int which, bool isGlobalOrMain) {
    string name = "";
    Type thisType = undefinedType;
    quaItem *thisItem = nullptr;
    if (which == 1) {
        thisItem = ptr->getItem1();
    } else if (which == 2) {
        thisItem = ptr->getItem2();
    } else if (which == 3) {
        thisItem = ptr->getItem3();
    } else {
        printf("in getAddress, 'which' is not in range\n");
        return "0";
    }
    name = thisItem->getName();
    if (!isGlobalOrMain) {  //in a function
        string baseAddress = "";
        for (int i = functionMapping.size() - 1; i >= 0; i--) {
            if (functionMapping[i].name == name) {
                baseAddress = functionMapping[i].address;
                return baseAddress + "($sp)";
            }
        }
        for (int i = globalMapping.size() - 1; i >= 0; i--) {
            if (globalMapping[i].name == name) {
                baseAddress = globalMapping[i].address;
                return baseAddress;
            }
        }
        printf("[WARNING!!!!, in function ,some item no in global and function mapping], name is : %s\n", name.c_str());
        return "0x00000000";
    } else if (isGlobalOrMain) { //in main
        string baseAddress = "";
        for (int i = globalMapping.size() - 1; i >= 0; i--) {
            if (globalMapping[i].name == name) {
                baseAddress = globalMapping[i].address;
                return baseAddress;
            }
        }
        printf("[WARNING!!!!, in main ,some item no in global mapping], name is : %s\n", name.c_str());
        return "0x00000000";
    }
}

int get_num_address(quadruple *ptr, int which) {
    quaItem *thisItem = nullptr;
    if (which == 1) {
        thisItem = ptr->getItem1();
    } else if (which == 2) {
        thisItem = ptr->getItem2();
    } else if (which == 3) {
        thisItem = ptr->getItem3();
    } else {
        printf("in get_num_address, 'which' is not in range\n");
        return 0;
    }
    string name = thisItem->getName();
    if (functionMapping.size() != 0) {  //in a function
        string baseAddress = "";
        for (int i = functionMapping.size() - 1; i >= 0; i--) {
            if (functionMapping[i].name == name) {
                baseAddress = functionMapping[i].address;
                int row = thisItem->getRowNum();
                int col = thisItem->getColNum();
                if (row != -1 && col != -1) { //a[1][2] = 3
                    int col_num = functionMapping[i].nameTemplate->getColNum();
                    return parseAddress(baseAddress) + (row * col_num + col) * 4;
                } else if (row != -1 && col == -1) { //a[3] = 4
                    return parseAddress(baseAddress) + row * 4;
                } else
                    return parseAddress(baseAddress);
            }
        }
        for (int i = globalMapping.size() - 1; i >= 0; i--) {
            if (globalMapping[i].name == name) {
                baseAddress = globalMapping[i].address;
                int row = thisItem->getRowNum();
                int col = thisItem->getColNum();
                if (row != -1 && col != -1) { //a[1][2] = 3
                    int col_num = globalMapping[i].nameTemplate->getColNum();
                    return parseAddress(baseAddress) + (row * col_num + col) * 4;
                } else if (row != -1 && col == -1) { //a[3] = 4
                    return parseAddress(baseAddress) + row * 4;
                } else
                    return parseAddress(baseAddress);
            }
        }
        printf("[WARNING!!!!, in function, get_num_address,some item no in global and function mapping], name is : %s\n", name.c_str());
        return 0;
    } else { // in main
        string baseAddress = "";
        for (int i = globalMapping.size() - 1; i >= 0; i--) {
            if (globalMapping[i].name == name) {
                baseAddress = globalMapping[i].address;
                int row = thisItem->getRowNum();
                int col = thisItem->getColNum();
                if (row != -1 && col != -1) { //a[1][2] = 3
                    int col_num = globalMapping[i].nameTemplate->getColNum();
                    return parseAddress(baseAddress) + (row * col_num + col) * 4;
                } else if (row != -1 && col == -1) { //a[3] = 4
                    return parseAddress(baseAddress) + row * 4;
                } else
                    return parseAddress(baseAddress);
            }
        }
        printf("[WARNING!!!!, in main, get_num_address,some item no in global and function mapping], name is : %s\n", name.c_str());
        return 0;
    }
}

string getBaseAddressByName(string name) {
    if (functionMapping.size() != 0) {  //in a function
        string baseAddress = "";
        for (int i = functionMapping.size() - 1; i >= 0; i--) {
            if (functionMapping[i].name == name) {
                baseAddress = functionMapping[i].address;
                return baseAddress;
            }
        }
        for (int i = globalMapping.size() - 1; i >= 0; i--) {
            if (globalMapping[i].name == name) {
                baseAddress = globalMapping[i].address;
                return baseAddress;
            }
        }
        printf("[WARNING!!!!, in function, getBaseAddressByName,some item no in global and function mapping], name is : %s\n", name.c_str());
        return 0;
    } else { // in main
        string baseAddress = "";
        for (int i = globalMapping.size() - 1; i >= 0; i--) {
            if (globalMapping[i].name == name) {
                baseAddress = globalMapping[i].address;
                return baseAddress;
            }
        }
        printf("[WARNING!!!!, in main, getBaseAddressByName item no in global and function mapping], name is : %s\n", name.c_str());
        return 0;
    }
}

int found_in_function_map(string name) {
    for (int i = functionMapping.size() - 1; i >= 0; i--) {
        if (functionMapping[i].name == name) {
            return 1;
        }
    }
    return 0;
}

quaItem* getTemplate(string name) {
    if (functionMapping.size() != 0) {
        for (int i = functionMapping.size() - 1; i >= 0; i--) {
            if (functionMapping[i].name == name)
                return functionMapping[i].nameTemplate;
        }
        for (int i = globalMapping.size() - 1; i >= 0; i--) {
            if (globalMapping[i].name == name)
                return globalMapping[i].nameTemplate;
        }
        printf("template not found, in function, name is : %s", name.c_str());
        return nullptr;
    } else {
        for (int i = globalMapping.size() - 1; i >= 0; i--) {
            if (globalMapping[i].name == name)
                return globalMapping[i].nameTemplate;
        }
        printf("template not found, in main, name is : %s", name.c_str());
        return nullptr;
    }
}

int parseAddress(string str) {
    int address = 0, i = 0;
    for (i = 0; i < str.length(); i++)
        address = address * 10 + (str[i] - '0');
    return address;
}

void printFunctionMapping() {
    if (!mappingDebug)
        return ;
    for (int i = 0; i < functionMapping.size(); i++) {
        printf("[functionMapping] %s\t in\t %s\n", functionMapping[i].name.c_str(), functionMapping[i].address.c_str());
    }
}

void printGlobalMapping() {
    if (!mappingDebug)
        return ;
    for (int i = 0; i < globalMapping.size(); i++) {
        printf("[globalMapping] %s\t in\t %s\n", globalMapping[i].name.c_str(), globalMapping[i].address.c_str());
    }
}

