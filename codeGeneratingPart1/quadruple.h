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
    void setType(Type type);
    quaItem(string name, Type type);

private:
    string name;
    Type type;
};

class quadruple
{
public:
    string getName();
    Type getType();
    quaItem* toQuaItem();
    quadruple(string op, quaItem *item1, quaItem *item2);
    string toString();
    static int quadrupleCnt;
    quadrupleKind getKind();
    void generateCode(FILE *OUT);
    quaItem* getItem1();
    quaItem* getItem2();
    quaItem* getItem3();

private:
    string op;
    quaItem *item1 = nullptr;
    quaItem *item2 = nullptr;
    quaItem *item3 = nullptr;
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
#endif //CODEGENERATING_QUADRUPLE_H
