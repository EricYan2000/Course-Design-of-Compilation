//
// Created by SAMSUNG on 2020/11/10.
//

#include "quadruple.h"
#include "config.h"
#include "memMap.h"
#include <stdlib.h>


//=======quaItem starts from here=======
quaItem::quaItem(string name, Type type) {
    this->name = name;
    this->type = type;
}

string quaItem::getName() {
    return this->name;
}

Type quaItem::getType() {
    return this->type;
}

void quaItem::setType(Type type) {
    this->type = type;
}

//=====quadruple starts from here=====
quadruple::quadruple(string op, quaItem *item1, quaItem *item2) {
    this->op = op;
    this->item1 = item1;
    this->item2 = item2;
    Type item3Type = undefinedType;
    if(op == "+" || op == "-" || op == "*" || op == "/") {
        this->kind = calculateKind;
        item3Type = intType;
        quadrupleCnt++;
        this->item3 = new quaItem("T" + to_string(quadrupleCnt), item3Type);
    } else if (op == "printf") {
        this->kind = printfKind;
        item3Type = printfType;
        this->item3 = new quaItem("printf", item3Type);
    } else if (op == "scanf") {
        this->kind = scanfKind;
        item3Type = scanfType;
        this->item3 = new quaItem("scanf", item3Type);
    } else if (op == "defineConstInt") {
        this->kind = constIntDefinition;
        item3Type = constInt;
        this->item3 = item1;
    }  else if (op == "defineConstChar") {
        this->kind = constCharDefinition;
        item3Type = constChar;
        this->item3 = item1;
    } else if (op == "defineVarInt") {
        this->kind = varIntDefinition;
        this->item3 = item1;
    } else if (op == "defineVarChar") {
        this->kind = varCharDefinition;
        this->item3 = item1;
    } else if (op == "assign") {
        this->kind = assignKind;
        this->item3 = item2;
    }
    else
        printf("quadruple type undecided\n");
}

string quadruple::getName() {
    return this->item3->getName();
}

Type quadruple::getType() {
    return this->item3->getType();
}

quadrupleKind quadruple::getKind() {
    return this->kind;
}

quaItem* quadruple::toQuaItem() {
    return this->item3;
}

quaItem* quadruple::getItem1() {
    return this->item1;
}

quaItem* quadruple::getItem2() {
    return this->item2;
}

quaItem* quadruple::getItem3() {
    return this->item3;
}

string quadruple::toString() {
    if (this->kind == calculateKind) {
        if (this->item1 != nullptr && this->item2 != nullptr && this->item3 != nullptr)
            return this->item3->getName() + " = " + this->item1->getName() + " " + op
                   + " " + this->item2->getName() + "\n";
        else
            return "calculateKind toString not implemented\n";
    } else if (this->kind == printfKind) {
        string ret = this->op + " " + this->item1->getName(); //printf("hello")
        if (this->item2 == nullptr) {
            ret = ret + "\n";
        } else {
            ret = ret + " " + this->item2->getName() + "\n"; //print("hello", a)
        }
        return ret;
    } else if (this->kind == scanfKind) {
        return this->op + " " + this->item1->getName() + "\n";
    } else if (this->kind == constIntDefinition || this->kind == constCharDefinition) {
        string token = (this->item2->getType() == constInt) ? "int" : "char";
        return "const " + token + " " + this->item1->getName()
                    + " = " + this->item2->getName() + "\n";
    } else if (this->kind == varIntDefinition || this->kind == varCharDefinition) {
        string token = (this->item1->getType() == intType) ? "int" : "char";
        string ret =  "var " + token + " " + this->item1->getName();
        if (this->item2 != nullptr) {
            ret = ret  + " = " + this->item2->getName();
        }
        return ret + "\n";
    } else if (this->kind == assignKind) {
        return this->item3->getName() + " = " + this->item1->getName() + "\n";
    }
    else
        return "toString not implemented\n";
}

void quadruple::generateCode(FILE *mipsOUT) {
    if (this->kind == calculateKind) {
        if (this->item1->getType() == constInt || this->item1->getType() == constChar) {
            immToRegister(8, this->item1->getName(), mipsOUT);
        } else {
            loadToRegister(8, getAddress(this, 1), mipsOUT);
        }
        if (this->item2->getType() == constInt || this->item2->getType() == constChar) {
            immToRegister(9, this->item2->getName(), mipsOUT);
        } else {
            loadToRegister(9, getAddress(this, 2), mipsOUT);
        }
        if (this->op == "+") {
            fprintf(mipsOUT, "\tadd $10, $8, $9\n");
        } else if (this->op == "-") {
            fprintf(mipsOUT, "\tsub $10, $8, $9\n");
        } else if (this->op == "*") {
            fprintf(mipsOUT, "\tmult $8, $9\n");
            fprintf(mipsOUT, "\tmflo $10\n");
        } else if (this->op == "/") {
            fprintf(mipsOUT, "\tdiv $8, $9\n");
            fprintf(mipsOUT, "\tmflo $10\n");
        } else {
            printf("in generateCode, wrong op\n");
        }
        saveToMem(10, getAddress(this), mipsOUT);
    } else if (this->kind == printfKind) {
        if (this->item1->getType() != constString) { //printf(<expression>)
            Type thisType = this->item1->getType();
            string address = getAddress(this, 1); //item1
            if (thisType == intType)
                printInt(address, mipsOUT);
            else if (thisType == charType)
                printChar(address, mipsOUT);
            else if (thisType == constInt)
                printConstInt(this->getItem1()->getName(), mipsOUT);
            else if (thisType == constChar)
                printConstChar(this->item1->getName(), mipsOUT);
            else
                printf("in quadruple::generateCode---printfKind, what type??\n");
        } else {
            string address = getAddress(this, 1); //item1
            printString(address, mipsOUT);
            if (this->item2 != nullptr) { //<string>, [<expression>]
                Type thisType = this->item2->getType();
                string address = getAddress(this, 2); //item1
                if (thisType == intType)
                    printInt(address, mipsOUT);
                else if (thisType == charType)
                    printChar(address, mipsOUT);
                else if (thisType == constInt)
                    printConstInt(this->item2->getName(), mipsOUT);
                else if (thisType == constChar)
                    printConstChar(this->item2->getName(), mipsOUT);
                else
                    printf("in quadruple::generateCode---printfKind, what type??\n");
            }
        }
        printEnter(mipsOUT);
    } else if (this->kind == scanfKind) {
        Type thisType = this->item1->getType();
        string address = getAddress(this, 1); //item1
        if (thisType == intType) {
            readInt(address, mipsOUT);
        } else {
            readChar(address, mipsOUT);
        }
    } else if (this->kind == constIntDefinition || this->kind == constCharDefinition ||
                this->kind == varIntDefinition || this->kind == varCharDefinition) {
        if (this->item2 != nullptr) {
            string address = getAddress(this);
            loadConstToRegister(8, this->item2->getName(), mipsOUT);
            saveToMem(8, address, mipsOUT);
        }
    } else if (this->kind == assignKind) {
        if (this->item1->getType() == constInt || this->item1->getType() == constChar) {
            immToRegister(8, this->item1->getName(), mipsOUT);
        } else {
            string address1 = getAddress(this, 1);
            loadToRegister(8, address1, mipsOUT);
        }
        string address3 = getAddress(this, 3);
        saveToMem(8, address3, mipsOUT);
    } else
        printf("this kind never seen, in quadruple::generateCode()\n");
}

void immToRegister(int registerNo, string value, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tli $%d, %s\n", registerNo, value.c_str());
}

void loadToRegister(int registerNo, string address, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tlw $%d, %s\n", registerNo, address.c_str());
}

void saveToMem(int registerNo, string address, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tsw $%d, %s\n", registerNo, address.c_str());
}

void loadConstToRegister(int registerNo, string value, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tli $%d, %s\n", registerNo, value.c_str());
}

void printConstInt(string value, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tli $a0, %s\n", value.c_str());
    fprintf(mipsOUT, "\tli $v0, 1\n");
    fprintf(mipsOUT, "\tsyscall\n");
}

void printConstChar(string value, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tli $a0, %s\n", value.c_str());
    fprintf(mipsOUT, "\tli $v0, 11\n");
    fprintf(mipsOUT, "\tsyscall\n");
}

void printInt(string address, FILE *mipsOUT) {
    loadToRegister(4, address, mipsOUT); //$a0 = value
    fprintf(mipsOUT, "\tli $v0, 1\n");
    fprintf(mipsOUT, "\tsyscall\n");
}

void printChar(string address, FILE *mipsOUT) {
    loadToRegister(4, address, mipsOUT); //$a0 = value
    fprintf(mipsOUT, "\tli $v0, 11\n");
    fprintf(mipsOUT, "\tsyscall\n");
}

void printString (string address, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tla $a0, %s\n", address.c_str());
    fprintf(mipsOUT, "\tli $v0, 4\n");
    fprintf(mipsOUT, "\tsyscall\n");
}

void printEnter(FILE *mipsOUT) {
    fprintf(mipsOUT, "\tla $a0, enter\n");
    fprintf(mipsOUT, "\tli $v0, 4\n");
    fprintf(mipsOUT, "\tsyscall\n");
}

void readInt(string address, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tli $v0, 5\n");
    fprintf(mipsOUT, "\tsyscall\n");
    saveToMem(2, address, mipsOUT);
}

void readChar(string address, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tli $v0, 12\n");
    fprintf(mipsOUT, "\tsyscall\n");
    saveToMem(2, address, mipsOUT);
}

int getValue(quaItem *ptr) {
    if (ptr->getType() == constInt) {
        return string2num(ptr->getName());
    } else if (ptr->getType() == constChar) {
        return char2num(ptr->getName());
    }
    printf("in getValue, wrong type\n");
    return 0;
}

int string2num(string str) {
    int i = 0;
    int negFlag = 1;
    int value = 0;
    if (str[0] == '-') {
        negFlag = -1;
        i++;
    }
    for (i; i < str.length(); i++)
        value = value * 10 + (str[i] - '0');
    return value * negFlag;
}

int char2num(string str) {
    return str[1];
}





