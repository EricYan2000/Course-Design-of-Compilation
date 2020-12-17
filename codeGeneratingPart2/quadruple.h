//
// Created by SAMSUNG on 2020/11/10.
//

#ifndef CODEGENERATING_QUADRUPLE_H
#define CODEGENERATING_QUADRUPLE_H

#include <iostream>
#include "type.h"
#include <unordered_map>

using namespace std;

class quaItem
{
public:
    string getName();
    Type getType();
    int getUnitBytes();
    int getRowNum();
    int getColNum();
    void setType(Type type);
    int getSize();
    quaItem(string name, Type type, int unit_bytes = 4, int row_num = -1, int col_num = -1);

private:
    string name;
    Type type;
    int unit_bytes;
    int row_num;
    int col_num;
};

class quadruple
{
public:
    string getName();
    Type getType();
    quaItem* toQuaItem();
    quadruple(string op, quaItem *item1, quaItem *item2, quaItem *item3 = nullptr, string arrayName = "", Type arrayType = undefinedType);
    string toString();
    static int quadrupleCnt;
    quadrupleKind getKind();
    void generateCode(FILE *OUT, bool isGlobalOrMain);
    quaItem* getItem1();
    quaItem* getItem2();
    quaItem* getItem3();
    Type getArrayType();

private:
    string op;
    quaItem *item1 = nullptr;
    quaItem *item2 = nullptr;
    quaItem *item3 = nullptr;
    string arrayName = "";
    Type arrayType = undefinedType;
    quadrupleKind kind;
};

void loadToRegister(int registerNo, string address, FILE *mipsOUT);
void saveToMem(int registerNo, string address, FILE *mipsOUT);
void loadConstToRegister(int registerNo, string value, FILE *mipsOUT);
void immToRegister(int registerNo, string value, FILE *mipsOUT);
int getValue(quaItem *ptr);
int string2num(string str);
int char2num(string str);
void printInt(string address, FILE *mipsOUT);
void printChar(string address, FILE *mipsOUT);
void printString (string address, FILE *mipsOUT);
void printEnter(FILE *mipsOUT);
void readInt(string address, FILE *mipsOUT);
void readChar(string address, FILE *mipsOUT);
void printConstInt(string value, FILE *mipsOUT);
void printConstChar(string value, FILE *mipsOUT);

extern quaItem *placeHolderQuaItem;

#endif //CODEGENERATING_QUADRUPLE_H
