#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include "wordDealing.h"
#include "type.h"
#include "symbolTable.h"
#include "memMap.h"
#include <regex>

using namespace std;

unordered_map<string, vector<Type>> retFuncNameMap;
unordered_map<string, vector<Type>> noRetFuncNameMap;


//basic information
int grammarDealing();

int level = 0;
Type funcRetType = undefinedType;   //widely used in returnStatement, don't delete !!!!
int returnNum = 0;                  //widely used in 'functions', don't delete
bool inMainFunc = false;

//grammarDealing
void program();
void constDeclaration();
void varDeclaration();
void functions();
void mainFunction();
void constDefinition();
tablePt IDENFR(bool isDeclare = false);
int INTCON();
string CHARCON();
void STRCON();
bool validStr(string basicString);
int INTEGER();
void SEMICN();
void LPARENT();
void RPARENT();
void LBRACK();
void RBRACK();
void LBRACE();
void RBRACE();
void COMMA();
void ASSIGN();
void plusAndMinus();
Type funcHead();
void retFunc();
void noRetFunc();
vector<Type> funcTable(vector<Type> paraTable);
void compoundStatement();
void varDefinition();
string constVar(Type frontType = intCharType);
int isPresetVar();
void presetVarDefinition();
void noPresetVarDefinition();
void noPresetVarDefine(Type type);
void assignTwoDimensionWithConstVar(Type thisType, int row, int column);
void rowsOfStatements();
Type typeSymbol();
void retFuncStatement();
void noRetFuncStatement();
void addRetFunc(string funcName, vector<Type> vec);
bool isRetFuncRecorded(string funcName);
void addNoRetFunc(string funcName, vector<Type> vec);
bool isNoRetFuncRecorded(string funcName);
vector<Type> getNoRetFuncDefinedTypeVector(string funcName);
vector<Type> getRetFuncDefinedTypeVector(string funcName);
Type factor();
Type expression();
Type item();
void scanfStatement();
void printfStatement();
void returnStatement();
void statement();
void assignStatement();
void conditionStatement();
string condition(string labelHead, int cnt, int subIndex = -1);
string relationOp();
void loopStatement();
int stepWidth();
void switchStatement();
string switchTable(Type frontType, int thisCnt);
void switchDefault();
string subSwitch(Type frontType, int thisCnt, int subCnt, quaItem *expressionItem);
void inputFuncTable(vector<Type> defineTypeVector, bool isUndefined = false, string funcName = "");
bool searchForAssign();
void checkConstAssign(string identifierName);
void undefinedFunction();

// function's paraTable debug
void printParaTable();

// form quadruple
vector<quaItem*> quaItemStack;
void pushQuaItemStack(quaItem* quaItemPtr);
quaItem* popQuaItemStack();
vector<quadruple*> quadrupleStack;
void pushQuadrupleStack(quadruple* quadruplePtr);
void printQuaItemStack();
void generateQuadruple(string op, int quaItemNeeded, string arrayName = "", Type arrrayType = undefinedType);
void printBridgeCode();
void generateDefineQuadruple(defineType type, string name, string value);
void generateDefineArrayQuadruple(quaItem *arrayItem);
void generateMipsCode();
void generateDotData();
void appendDotData(quaItem *pItem);


string generate_label_then_branch(string opString, string name, int thisCnt, int subCnt = -1);
////basicString should be the original opToken
string set_label(string basicString, int cnt = -1);
string generate_label_and_jump(string labelName = "", int cnt = -1);
string jump_to_label(string labelName);
string jal_to_label(string labelName);

int getFuncSpace(string funcName);


static int msgCnt = 0;
static int ifCnt = 0;
static int whileCnt = 0;
static int forCnt = 0;
static int switchCnt = 0;
int quadruple::quadrupleCnt = 0;

int strCount = 2;
string reg1 = "\\\\";
regex r1(reg1);
string reg2 = "\\\\";


//===========================================================
//1. Add "\'" when you are adding charCon in to quaItem
//===========================================================

int main()
{
    /*
    unordered_map<string, string> test;
    test["111"] = "111";
    printf("%s\n", test["111"].c_str());
    printf("%d\n", test.size());
    printf("%s\n", test["123"].c_str());
    printf("%d\n", test.size());
     */
    generateDotData();

    wordDealing();

    grammarDealing();   //getSym() in its first line

    printQuaItemStack();

    printBridgeCode();

    printf("[WARINING!!] 现在的内存分配以及mapping没有考虑level不同的情况\n");

    printf("[=====function information=====]\n");
    for (int i = 0; i < functionNameVector.size(); i++) {
        string returnType = "";
        if (returnTypeVector[i] == intType)
            returnType = "int";
        else if (returnTypeVector[i] == charType)
            returnType = "char";
        else
            returnType = "void";
        printf("[%s]\treturnType %s\tspace %d\n", functionNameVector[i].c_str(), returnType.c_str(), spaceNeeded[i]);
    }
    printf("[===function information ends===]\n");

    globalTop += strCount;
    int remain = globalTop % 4;
    globalTop += (4 - remain);

    generateMipsCode();

    printGlobalMapping();

    printf("[!!!!finished!!!!]\n");

    return 0;
}

//generate mips code
void generateMipsCode() {
    printf("[generating mips code]\n");
    fprintf(mipsOUT, "\n.text\n");
    quadruple *thisQuadruple = nullptr;
    bool isGlobalOrMain = true;
    quadrupleKind thisKind = undefinedKind;
    for (int i = 0; i < quadrupleStack.size(); i++) {
        thisQuadruple = quadrupleStack[i];
        thisKind = thisQuadruple->getKind();
        if (thisKind == funcEndKind) {
            functionEnds();
            isGlobalOrMain = true;
        }
        if (thisKind == funcDefKind)
            isGlobalOrMain = false;
        if (thisKind == mainEntranceKind)
            isGlobalOrMain = true;
        if (thisKind == constIntDefinition || thisKind == constCharDefinition || thisKind == getReturnValueKind ||
            thisKind == varIntDefinition || thisKind == varCharDefinition || thisKind == calculateKind ||
            thisKind == useArrayKind || thisKind == paraDefKind) {
            string thisName = thisQuadruple->getName();
            assignSpace(thisName, 4, isGlobalOrMain);
        } else if (thisKind == intArrayDefinition || thisKind == charArrayDefinition) {
            assignSpace(thisQuadruple->getName(), thisQuadruple->getItem1()->getSize(), isGlobalOrMain, thisQuadruple->getItem3());
        }
        thisQuadruple->generateCode(mipsOUT, isGlobalOrMain);
    }
}

void generateDotData() {
    fprintf(mipsOUT, ".data\n");
    fprintf(mipsOUT, "\tenter: .asciiz \"\\n\"\n");
}


void appendDotData(quaItem *quaItemPtr) {
    msgCnt++;
    string originalString = quaItemPtr->getName();
    string newString = regex_replace(originalString, r1, reg2);
    string info = "\tmsg" + to_string(msgCnt) + ": .asciiz " + newString + "\n";
    strCount += strlen(originalString.c_str()) + 1;
    fprintf(mipsOUT, R"+*(%s)+*", info.c_str());
    addressCon thisCon = addressCon{
            .name = quaItemPtr->getName(),
            .address = "msg" + to_string(msgCnt),
            .nameTemplate = nullptr,
    };
    globalMapping.push_back(thisCon);
}

// form quadruple
void pushQuaItemStack(quaItem* quaItemPtr) {
    quaItemStack.push_back(quaItemPtr);
    if (quaItemPtr->getType() == constString)
        appendDotData(quaItemPtr);
}

quaItem* popQuaItemStack() {
    int index = quaItemStack.size();
    if (index <= 0) {
        printf("try to pop when quaItemStack is empty\n");
        return nullptr;
    }
    else {
        quaItem *ptr = quaItemStack[index - 1];
        quaItemStack.pop_back();
        return ptr;
    }
}

quaItem* topQuaItemStack() {
    return quaItemStack.back();
}

void pushQuadrupleStack(quadruple* quadruplePtr) {
    quadrupleStack.push_back(quadruplePtr);
    quadrupleKind thisKind = quadruplePtr->getKind();
    if (thisKind == constIntDefinition || thisKind == constCharDefinition ||
        thisKind == varIntDefinition || thisKind == varCharDefinition) {
        //assignSpace(quadruplePtr, 4);
    } else if (thisKind == calculateKind) {
        //assignSpace(quadruplePtr, 4);
        //register temporary var in the table to count space needed
        registerItem(quadruplePtr->getName(), intType, 4, false, false, level, 0);
    } else if (thisKind == intArrayDefinition || thisKind == charArrayDefinition) {
        //assignSpace(quadruplePtr, quadruplePtr->getItem1()->getSize());
    } else if (thisKind == useArrayKind) {
        //assignSpace(quadruplePtr, 4);
        //register temporary var in the table to count space needed
        registerItem(quadruplePtr->getName(), quadruplePtr->getArrayType(), 4, false, false, level, 0);
    } else if (thisKind == getReturnValueKind) {
        //assignSpace(quadruplePtr, 4);
        //register temporary var in the table to count space needed
        registerItem(quadruplePtr->getName(), quadruplePtr->getArrayType(), 4, false, false, level, 0);
    }
}

/*
void pushQuadrupleStack(quadruple* quadruplePtr) {
    quadrupleStack.push_back(quadruplePtr);
    quadrupleKind thisKind = quadruplePtr->getKind();
    if (thisKind == constIntDefinition || thisKind == constCharDefinition ||
        thisKind == varIntDefinition || thisKind == varCharDefinition) {
        assignSpace(quadruplePtr, 4);
    } else if (thisKind == calculateKind) {
        assignSpace(quadruplePtr, 4);
        //register temporary var in the table to count space needed
        registerItem(quadruplePtr->getName(), intType, 4, false, false, level, 0);
    } else if (thisKind == intArrayDefinition || thisKind == charArrayDefinition) {
        printf("in [pushQuadrupleStack], testing assigning space for array\n");
        assignSpace(quadruplePtr, quadruplePtr->getItem1()->getSize());
    }
}
 *
 */

void printQuaItemStack() {
    if (!itemStackDebug)
        return ;
    for (int i = 0; i < quaItemStack.size(); i++) {
        quaItem *ptr = quaItemStack[i];
        string type = "";
        if (ptr->getType() == undefinedType)
            type = "undefinedType";
        else if (ptr->getType() == intType)
            type = "intType";
        else if (ptr->getType() == charType)
            type = "charType";
        else if (ptr->getType() == voidType)
            type = "voidType";
        else
            type = "funcType";
        printf("%s %s\n", ptr->getName().c_str(), type.c_str());
    }
}

void generateQuadruple(string op, int quaItemNeeded, string arrayName, Type arrayType) {
    int length = quaItemStack.size();
    if (length < quaItemNeeded)
        printf("stack here fewer quaItem then needed\n");
    else {
        quadruple *quadruplePtr = nullptr;
        if (quaItemNeeded == 2) {
            quaItem *item2 = popQuaItemStack();
            quaItem *item1 = popQuaItemStack();
            quadruplePtr = new quadruple(op, item1, item2, nullptr, arrayName, arrayType); //do not switch sequence
        } else if (quaItemNeeded == 1) {
            quadruplePtr = new quadruple(op, popQuaItemStack(), nullptr, nullptr, arrayName, arrayType); //do not switch sequence
        } else if (quaItemNeeded == 3) {
            quaItem *item3 = popQuaItemStack();
            quaItem *item2 = popQuaItemStack();
            quaItem *item1 = popQuaItemStack();
            quadruplePtr = new quadruple(op, item1, item2, item3, arrayName, arrayType); //do not switch sequence
        } else if (quaItemNeeded == 0) {
            quadruplePtr = new quadruple(op, nullptr, nullptr, nullptr, arrayName, arrayType); //do not switch sequence
        } else {
            printf("quaItemNeeded number undefined, in generateQuadruple\n");
        }
        pushQuadrupleStack(quadruplePtr);
        if (quadruplePtr->getKind() != funcCallKind && quadruplePtr->getKind() != paraInputKind
            && quadruplePtr->getKind() != jalKind && quadruplePtr->getKind() != funcCallHeadMarkKind) // 去重
            pushQuaItemStack(quadruplePtr->toQuaItem());
    }
}

void printBridgeCode() {
    if (!bridgeCodeDebug)
        return ;
    for (int i = 0; i < quadrupleStack.size(); i++)
        printf("%s", quadrupleStack[i]->toString().c_str());
}

// form quadruple ends here

//basic functions
int grammarDealing()
{
    getSym();
    program();
    if (paraTableDebug) {
        printParaTable();
    }
    return 0;
}

void  printParaTable() {
    vector<Type> paraTable;
    string name = "";
    printf("noRetFunc\n");
    for (auto i = noRetFuncNameMap.begin(); i != noRetFuncNameMap.end(); i++) {
        name = i->first;
        paraTable = i->second;
        printf("[%s] ", name.c_str());
        for (int j = 0; j < paraTable.size(); j++)
            printf("%s ", (paraTable[j] == intType) ? "int" : "char");
        printf("\n");
    }
    printf("retFunc\n");
    for (auto i = retFuncNameMap.begin(); i != retFuncNameMap.end(); i++) {
        name = i->first;
        paraTable = i->second;
        printf("[%s] ", name.c_str());
        for (int j = 0; j < paraTable.size(); j++)
            printf("%s ", (paraTable[j] == intType) ? "int" : "char");
        printf("\n");
    }
}

void SEMICN() {
    if (thisSym == "SEMICN") {
        getSym();
    } else {
        error("k", peekLine(-1));
    }
}

void LPARENT() {
    if (thisSym == "LPARENT") {
        getSym();
    } else {
        error("missing LPARENT");
    }
}

void RPARENT() {
    if (thisSym == "RPARENT") {
        getSym();
    } else {
        error("l", peekLine(-1));
    }
}

void LBRACK() {
    if (thisSym == "LBRACK") {
        getSym();
    } else {
        error("missing LBRACK");
    }
}

void RBRACK() {
    if (thisSym == "RBRACK") {
        getSym();
    } else {
        error("m", peekLine(-1));
    }
}

void LBRACE() {
    if (braceDebug)
        printf("line %d should have LBRACE\n", thisLine);
    if (thisSym == "LBRACE") {
        getSym();
    } else {
        error("missing LBRACE");
    }
}

void RBRACE() {
    //see noRetFunc() and retFunc()
    if (braceDebug)
        printf("line %d should have RBRACE\n", thisLine);
    if (thisSym == "RBRACE") {
        getSym();
    } else {
        error("missing RBRACE", peekLine(-1));
    }
}

void ASSIGN() {
    if (thisSym == "ASSIGN") {
        getSym();
    } else {
        error("missing ASSIGN");
    }
}

void COMMA() {
    if (thisSym == "COMMA") {
        getSym();
    } else {
        error("missing COMMA");
    }
}

void addRetFunc(string funcName, vector<Type> vec) {
    string str = funcName;
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    retFuncNameMap[str] = vec;
}

bool isRetFuncRecorded(string funcName) {
    string str = funcName;
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return retFuncNameMap.count(str) != 0;
}

void addNoRetFunc(string funcName, vector<Type> vec) {
    string str = funcName;
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    noRetFuncNameMap[str] = vec;
}

bool isNoRetFuncRecorded(string funcName) {
    string str = funcName;
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return noRetFuncNameMap.count(str) != 0;
}

void program() {
    if (thisSym == "CONSTTK") {
        constDeclaration();
    }
    if ((thisSym == "INTTK" || thisSym == "CHARTK") && peekSym(2) != "LPARENT") { //make sure not function
        varDeclaration();
    }
    jump_to_label("main_Entrance");
    while((thisSym == "INTTK" || thisSym == "CHARTK" || thisSym == "VOIDTK")
          && peekSym(2) == "LPARENT" && peekSym(1) == "IDENFR") { //functions  // &&peekSym(1) == "IDENFR"
        if (thisSym == "INTTK")
            returnTypeVector.push_back(intType);
        else if (thisSym == "CHARTK")
            returnTypeVector.push_back(charType);
        else
            returnTypeVector.push_back(voidType);
        functions();
        popTable(level);
    }
    returnTypeVector.push_back(voidType);
    functionNameVector.push_back("main");
    inMainFunc = true;
    mainFunction();
    inMainFunc = false;
    popTable(level);
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<程序>\n");
}

void constDeclaration() {
    if (thisSym == "CONSTTK") {
        getSym();
        constDefinition();
        SEMICN();
    }
    while (thisSym == "CONSTTK") { //extra consts
        getSym();
        constDefinition();
        SEMICN();
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<常量说明>\n");
}

void constDefinition() {
    string name = "";
    if (thisSym == "INTTK") { //int a = 10, b = 15;
        Type thisType = intType;
        getSym();
        name = IDENFR(true)->codeName;
        registerItem(name, thisType, 4, true, false, level, 0);
        ASSIGN();
        int value = INTEGER();
        generateDefineQuadruple(defineConstInt, name, to_string(value));
        while (thisSym == "COMMA") {
            getSym();
            name = IDENFR(true)->codeName;
            registerItem(name, thisType, 4, true, false, level, 0);
            ASSIGN();
            value = INTEGER();
            generateDefineQuadruple(defineConstInt, name, to_string(value));
        }
    } else if (thisSym == "CHARTK") { //char c = 'a', p = 'z'
        Type thisType = charType;
        getSym();
        name = IDENFR(true)->codeName;
        registerItem(name, thisType, 4, true, false, level, 0);
        ASSIGN();
        string value = CHARCON();
        generateDefineQuadruple(defineConstChar, name, value);
        while (thisSym == "COMMA") {
            COMMA();
            name = IDENFR(true)->codeName;
            registerItem(name, thisType, 4, true, false, level, 0);
            ASSIGN();
            value = CHARCON();
            generateDefineQuadruple(defineConstChar, name, value);
        }
    } else
        error();
    if (grammarDealingDebug && thisSym != "COMMA")
        fprintf(grammarDealingOUT, "<常量定义>\n");
}

void generateDefineQuadruple(defineType defineType, string name, string value) {
    if (defineType == defineConstInt) {
        pushQuaItemStack(new quaItem(name, constInt));
        pushQuaItemStack(new quaItem(value, constInt));
        generateQuadruple("defineConstInt", 2);
    } else if (defineType == defineConstChar) {
        pushQuaItemStack(new quaItem(name, constChar));
        pushQuaItemStack(new quaItem("\'" + value + "\'", constChar));
        generateQuadruple("defineConstInt", 2);
    } else if (defineType == defineVarInt) {
        if (value.length() != 0) {
            pushQuaItemStack(new quaItem(name, intType));
            pushQuaItemStack(new quaItem(value, constInt));
            generateQuadruple("defineVarInt", 2);
        } else {
            pushQuaItemStack(new quaItem(name, intType));
            generateQuadruple("defineVarInt", 1);
        }
    } else if (defineType == defineVarChar) {
        if (value.length() != 0) {
            pushQuaItemStack(new quaItem(name, charType));
            pushQuaItemStack(new quaItem("\'" + value + "\'", constChar));
            generateQuadruple("defineVarChar", 2);
        } else {
            pushQuaItemStack(new quaItem(name, charType));
            generateQuadruple("defineVarChar", 1);
        }
    }
}

void generateDefineArrayQuadruple(quaItem *arrayItem) {
    Type thisType = arrayItem->getType();
    if (thisType == intArrayType) {
        pushQuaItemStack(arrayItem);
        generateQuadruple("defineIntArray", 1);
    } else if (thisType == charArrayType) {
        pushQuaItemStack(arrayItem);
        generateQuadruple("defineCharArray", 1);
    }
}

void varDeclaration() {
    if ((thisSym == "INTTK" || thisSym == "CHARTK")
        && peekSym(1) == "IDENFR" && peekSym(2) != "LPARENT") { //make sure it's not a function
        varDefinition();
        SEMICN();
        while ((thisSym == "INTTK" || thisSym == "CHARTK")
               && peekSym(1) == "IDENFR" && peekSym(2) != "LPARENT") {
            varDefinition();
            SEMICN();
        }
    } else
        error();
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<变量说明>\n");
}

void varDefinition() {
    if (isPresetVar())
        presetVarDefinition();
    else
        noPresetVarDefinition();
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<变量定义>\n");
}

int isPresetVar() {
    int flag = 0;
    if (thisSym == "INTTK" || thisSym == "CHARTK") {
        int i = 1;
        while (peekSym(i) != "SEMICN" && peekSym(i) != "COMMA") { //read to the end of the item(or sentence)
            if (peekSym(i) == "ASSIGN") //if there's a '='
                flag = 1;
            i++;
        }
    }
    return flag;
}

void presetVarDefinition() {
    string name = "";
    string value = "";
    Type thisType = typeSymbol(); //getSym() and error() inside
    name = IDENFR(true)->codeName;
    if (thisSym == "ASSIGN") {
        registerItem(name, thisType, 4, false, false, level, 0);
        ASSIGN();
        value = constVar(thisType);   //end of the zero-dimension, like int a = 10;
        defineType thisDefineType = (thisType == intType) ? defineVarInt : defineVarChar;
        generateDefineQuadruple(thisDefineType, name, value);
    } else { //int a[
        LBRACK();
        int row_num = INTCON();
        RBRACK(); //int a[5]
        if (thisSym == "ASSIGN") { //int a[5] =
            Type arrayType = (thisType == intType) ? intArrayType : charArrayType;
            quaItem *arrayItem = new quaItem(name, arrayType, 4, row_num);
            registerItem(name, thisType, 4 * row_num, false, false, level, 1, arrayItem);
            generateDefineArrayQuadruple(arrayItem);
            ASSIGN();
            LBRACE(); //int a[5] = {
            value = constVar(thisType);
            int cntRow = 1;
            pushQuaItemStack(new quaItem(name, arrayType, 4, cntRow - 1));
            if (thisType == intType) {
                pushQuaItemStack(new quaItem(value, constInt, 4));
            } else {
                pushQuaItemStack(new quaItem("\'" + value + "\'", constChar, 4));
            }
            generateQuadruple("assign", 2);
            while (thisSym == "COMMA") {
                cntRow++;
                getSym();
                value = constVar(thisType);
                pushQuaItemStack(new quaItem(name, arrayType, 4, cntRow - 1));
                if (thisType == intType) {
                    pushQuaItemStack(new quaItem(value, constInt, 4));
                } else {
                    pushQuaItemStack(new quaItem("\'" + value + "\'", constChar, 4));
                }
                generateQuadruple("assign", 2);
            }
            if (cntRow != row_num)
                error("n", thisLine);
            RBRACE(); //int a[5] = {1,2}
        } else { //int a[5][
            LBRACK();
            int col_num = INTCON();
            RBRACK(); //int a[5][7]
            ASSIGN();
            Type arrayType = (thisType == intType) ? intArrayType : charArrayType;
            quaItem *arrayItem = new quaItem(name, arrayType, 4, row_num, col_num);
            registerItem(name, thisType, 4 * row_num * col_num, false, false, level, 2, arrayItem);
            generateDefineArrayQuadruple(arrayItem);
            int cntRow = 0;
            int cntCol = 0;
            LBRACE();
            LBRACE(); //int a[5][7] = {{
            cntRow++;
            value = constVar(thisType);
            cntCol++;
            pushQuaItemStack(new quaItem(name, arrayType, 4, cntRow - 1, cntCol - 1));
            if (thisType == intType) {
                pushQuaItemStack(new quaItem(value, constInt, 4));
            } else {
                pushQuaItemStack(new quaItem("\'" + value + "\'", constChar, 4));
            }
            generateQuadruple("assign", 2);
            while (thisSym == "COMMA") {
                getSym();
                value = constVar(thisType);
                cntCol++;
                pushQuaItemStack(new quaItem(name, arrayType, 4, cntRow - 1, cntCol - 1));
                if (thisType == intType) {
                    pushQuaItemStack(new quaItem(value, constInt, 4));
                } else {
                    pushQuaItemStack(new quaItem("\'" + value + "\'", constChar, 4));
                }
                generateQuadruple("assign", 2);
            }
            if (cntCol != col_num)
                error("n", thisLine);
            RBRACE();
            while (thisSym == "COMMA") { //int a[5][7] = {{1,2} ,
                cntCol = 0;
                getSym();
                LBRACE(); //int a[5][7] = {{1,2} , {
                cntRow++;
                value = constVar(thisType);
                cntCol++;
                pushQuaItemStack(new quaItem(name, arrayType, 4, cntRow - 1, cntCol - 1));
                if (thisType == intType) {
                    pushQuaItemStack(new quaItem(value, constInt, 4));
                } else {
                    pushQuaItemStack(new quaItem("\'" + value + "\'", constChar, 4));
                }
                generateQuadruple("assign", 2);
                while (thisSym == "COMMA") { //int a[5][7] = {{1,2} , {22,3,
                    getSym();
                    value = constVar(thisType);
                    cntCol++;
                    pushQuaItemStack(new quaItem(name, arrayType, 4, cntRow - 1, cntCol - 1));
                    if (thisType == intType) {
                        pushQuaItemStack(new quaItem(value, constInt, 4));
                    } else {
                        pushQuaItemStack(new quaItem("\'" + value + "\'", constChar, 4));
                    }
                    generateQuadruple("assign", 2);
                }
                if (cntCol != col_num)
                    error("n", thisLine);
                RBRACE(); //int a[5][7] = {{1,2} , {22,3,4}
            }
            RBRACE(); //should be int a[5][7] = {{1,2} , {22,3,4}}
            if (cntRow != row_num)
                error("n", peekLine(-1));
        }
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<变量定义及初始化>\n");
}
/*
Type arrayType = (thisType == intType) ? intArrayType : charArrayType;
quaItem *arrayItem = new quaItem(name, arrayType, 4, row_num, col_num);
registerItem(name, thisType, false, false, level, 2, arrayItem);
*/
void assignTwoDimensionWithConstVar(Type thisType, int row, int column) { //int a[5][7] =  //used in presetVarDefinition

}

void noPresetVarDefinition() {
    Type thisType = typeSymbol();
    noPresetVarDefine(thisType);
    while (thisSym == "COMMA") { //extra-length
        getSym();
        noPresetVarDefine(thisType);
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<变量定义无初始化>\n");
}

void noPresetVarDefine(Type type) {
    Type thisType = type;
    string name = IDENFR(true)->codeName; //zero-dimension, like c
    if (thisSym == "LBRACK") {
        LBRACK();
        int row_num = INTCON();
        RBRACK(); //end of the one-dimension, like c[10]
        if (thisSym == "LBRACK") { //two-dimension. like char c[5][10]
            LBRACK();
            int col_num = INTCON();
            Type arrayType = (thisType == intType) ? intArrayType : charArrayType;
            quaItem *arrayItem = new quaItem(name, arrayType, 4, row_num, col_num);
            registerItem(name, thisType, 4 * row_num * col_num, false, false, level, 2, arrayItem);
            generateDefineArrayQuadruple(arrayItem);
            RBRACK();
        } else { //one-dimension. like char c[10]
            Type arrayType = (thisType == intType) ? intArrayType : charArrayType;
            quaItem *arrayItem = new quaItem(name, arrayType, 4, row_num);
            registerItem(name, thisType, 4 * row_num, false, false, level, 1, arrayItem);
            generateDefineArrayQuadruple(arrayItem);
        }
    } else { //zero-dimension. like char c,
        registerItem(name, thisType, 4, false, false, level, 0);
        defineType thisDefineType = (thisType == intType) ? defineVarInt : defineVarChar;
        generateDefineQuadruple(thisDefineType, name, "");
    }
}

string constVar(Type frontType) {
    string value = "";
    if (thisSym == "CHARCON") {
        if (frontType != intCharType && frontType != charType)
            error("o", thisLine);
        value = CHARCON();
    } else {
        if (frontType != intCharType && frontType != intType)
            error("o", thisLine);
        int num = INTEGER(); //can throw error
        value = to_string(num);
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<常量>\n");
    return value;
}

int INTEGER() {    //SIGNED AND UNSIGNED INTEGER
    int flag = (thisSym == "MINU") ? -1 : 1;
    if (thisSym == "PLUS" || thisSym == "MINU") {
        plusAndMinus();
    }
    int num = flag * INTCON(); //can throw error
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<整数>\n");
    return num;
}

int INTCON() {  //UNSIGNED INTEGER
    string strNum = thisWord;
    int num = 0;
    char digit = 0;
    for (int i = 0; i < strNum.size(); i++) {
        digit = strNum[i];
        num = num * 10 + (digit - '0');
    }
    if (thisSym == "INTCON") {
        getSym();
    }
    else
        error();
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<无符号整数>\n");
    return num;
}

string CHARCON() {
    char c = thisWord[0];
    bool valid = (c == '+' || c == '-' || c == '*' || c == '/' || c == '_' || isalnum(c));
    if (thisSym == "CHARCON" && valid) {
        getSym();
    } else if (thisSym == "CHARCON" && !valid) {
        getSym();
        error("a", peekLine(-1));
    } else
        error("a", peekLine(-1));
    return string(1,c);
}

void STRCON() {
    if (thisSym != "STRCON") {
        error("a", peekLine(-1));
    } else if (thisSym == "STRCON" && !validStr(thisWord)) {
        error("a", peekLine(-1));
        getSym();
    } else {
        getSym();
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<字符串>\n");
}

bool validStr(string str) {
    if (str.size() == 0)
        return false;
    for (int i = 0; i < str.size(); i++)
        if (!(str[i] == 32 || str[i] == 33 || (str[i] >= 35 && str[i] <= 126)))
            return false;
    return true;
}

tablePt IDENFR(bool isDeclare) {
    tablePt pt = NULL;
    setItem(&searchItem, thisWord, undefinedType, 4, false, false, level, 0);
    if (!isDeclare) { //is not declare
        pt = find(searchItem);
        if (pt == &unfoundItem)
            error("c", thisLine);
    } else { //第一次定义，用于把名字返回会去。只要isDeclare为true，那么pt一定不会为空
        pt = &searchItem;
    }
    if (thisSym == "IDENFR") {
        getSym();
    } else {
        error();
    }
    return pt;
}

Type funcHead() {
    Type thisFuncType = undefinedType;
    if (thisSym == "INTTK" || thisSym == "CHARTK") {
        thisFuncType = (thisSym == "INTTK") ? intType : charType;
        getSym();IDENFR(true);
    } else
        error("funcHead doesn't have int or char");
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<声明头部>\n");
    return thisFuncType;
}

void functions() {  //self-created function
    if (thisSym == "VOIDTK") {
        funcRetType = voidType; //DON'T DELETE. USE IN returnStatement
        noRetFunc();
        funcRetType = undefinedType;
    } else if (thisSym == "INTTK") {
        returnNum = 0;
        funcRetType = intType; //DON'T DELETE. USE IN returnStatement
        retFunc();
        funcRetType = undefinedType;
        if (returnNum == 0)
            error("h", peekLine(-1)); //在右括号那行报错
    } else if (thisSym == "CHARTK") {
        returnNum = 0;
        funcRetType = charType; //DON'T DELETE. USE IN returnStatement
        retFunc();
        funcRetType = undefinedType;
        if (returnNum == 0)
            error("h", peekLine(-1)); //在右括号那行报错
    } else
        error("wrong function typeSymbol");
}

void noRetFunc() {
    //check 有没有重复
    string funcName = peekWord(1);
    functionNameVector.push_back(funcName);
    int cnt = noRetFuncNameMap.count(funcName);  // != 0 means, there's already a legal function calls funcName
    vector<Type> paraTable;
    if (cnt == 0)
        addNoRetFunc(funcName, paraTable); //嵌套问题，不可以换地方
    pushQuaItemStack(new quaItem(funcName, funcType, 0));
    generateQuadruple("funcDef", 1);
    if (thisSym == "VOIDTK") {
        getSym();
        IDENFR(true);
        registerItem(funcName, voidType, 0, false, true, level, 0);
        LPARENT();
        paraTable = funcTable(paraTable);
        if (cnt == 0)
            addNoRetFunc(funcName, paraTable); //嵌套问题，不可以换地方
        RPARENT();
        LBRACE();
        level++;
        compoundStatement();
        level--;
        RBRACE();
        paraCntVector.push_back(paraTable.size());
    } else {
        error();
    }
    generateQuadruple("funcEnd", 0, funcName);
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<无返回值函数定义>\n");
}

void retFunc() {
    string funcName = peekWord(1);
    functionNameVector.push_back(funcName);
    int cnt = retFuncNameMap.count(funcName);  // != 0 means, there's already a legal function calls funcName
    //check 有没有重复
    vector<Type> paraTable;
    if (cnt == 0)
        addRetFunc(funcName, paraTable); //解决fib嵌套定义的问题，不可以换地方
    pushQuaItemStack(new quaItem(funcName, funcType, 0));
    generateQuadruple("funcDef", 1);
    int errorFlag = 0;
    Type thisFuncType = funcHead();
    registerItem(funcName, thisFuncType, 0, false, true, level, 0);
    LPARENT();
    paraTable = funcTable(paraTable);
    if (cnt == 0)
        addRetFunc(funcName, paraTable); //解决fib嵌套定义的问题，不可以换地方
    RPARENT();
    LBRACE();
    level++;
    compoundStatement();
    level--;
    RBRACE();
    paraCntVector.push_back(paraTable.size());
    generateQuadruple("funcEnd", 0, funcName);
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<有返回值函数定义>\n");
}

void compoundStatement() {
    if (thisSym == "CONSTTK") {
        constDeclaration();
    }
    if ((thisSym == "INTTK" || thisSym == "CHARTK") && peekSym(1) == "IDENFR" && peekSym(2) != "LPARENT") {
        varDeclaration();
    }
    rowsOfStatements();
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<复合语句>\n");
}

void rowsOfStatements() {
    while (thisSym == "FORTK" || thisSym == "WHILETK" || thisSym == "IFTK" || thisSym == "SWITCHTK"
           || thisSym == "SEMICN" || thisSym == "RETURNTK" || thisSym == "LBRACE"
           || thisSym == "SCANFTK" || thisSym == "PRINTFTK"
           || (thisSym == "IDENFR" && peekSym(1) == "LPARENT")
           || (thisSym == "IDENFR" && searchForAssign()))
        statement();
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<语句列>\n");
}

bool searchForAssign() {
    int i = 0;
    string tempSym = peekSym(i);
    while (tempSym != "COMMA" && tempSym != "SEMICN") {
        if (tempSym == "ASSIGN")
            return true;
        i++;
        tempSym = peekSym(i);
    }
    return false;
}

void statement() {
    if (thisSym == "WHILETK" || thisSym == "FORTK") {
        loopStatement();
    } else if (thisSym == "IFTK") {
        conditionStatement();
    } else if (thisSym == "IDENFR" && isRetFuncRecorded(thisWord) && peekSym(1) == "LPARENT") {
        retFuncStatement();
        SEMICN();
    } else if (thisSym == "IDENFR" && isNoRetFuncRecorded(thisWord) && peekSym(1) == "LPARENT") {
        noRetFuncStatement();
        SEMICN();
    } else if (thisSym == "IDENFR" && peekSym(1) == "LPARENT") {
        undefinedFunction();
        SEMICN();
    } else if (thisSym == "SEMICN") { //<空>;
        SEMICN();
    } else if (thisSym == "RETURNTK") {
        returnStatement();
        SEMICN();
    } else if (thisSym == "SCANFTK") {
        scanfStatement();
        SEMICN();
    } else if (thisSym == "PRINTFTK") {
        printfStatement();
        SEMICN();
    } else if (thisSym == "SWITCHTK") {
        switchStatement();
    } else if (thisSym == "LBRACE") {
        LBRACE();
        rowsOfStatements();
        RBRACE();
    } else {
        assignStatement();
        SEMICN();
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<语句>\n");
}

void assignStatement() {
    tableItem *item = IDENFR();     //if error occurs, item = Null, and will jam the following new quaItem
    string thisName = item->codeName;
    Type thisType = item->type;
    checkConstAssign(peekWord(-1));
    if (thisSym == "ASSIGN") { //a = 1 + 2;
        ASSIGN();
        pushQuaItemStack(new quaItem(thisName, thisType));
        expression();
        generateQuadruple("assign", 2);
    } else {
        Type indexType = undefinedType;
        LBRACK();
        indexType = expression();
        if (indexType != intType)
            error("i", thisLine);
        RBRACK();
        if (thisSym == "ASSIGN") { //a[5] =
            ASSIGN();
            pushQuaItemStack(placeHolderQuaItem);
            expression();
            generateQuadruple("assignArray", 3, thisName, thisType);
        } else { //a[5][
            LBRACK();
            indexType = expression();
            if (indexType != intType)
                error("i", thisLine);
            RBRACK(); //a[5][6]
            ASSIGN();
            expression();
            generateQuadruple("assignArray", 3, thisName, thisType);
        }
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<赋值语句>\n");
}

void conditionStatement() { //if(....) do1  j endif\\ ifEnd: do2  endIf. endIf is the end of the entire conditionStatement
    ifCnt++;
    int thisCnt = ifCnt;    //necessary, in case of 'if' under 'if'
    if (thisSym == "IFTK") {
        getSym();
        LPARENT();
        string ifEnd = condition("ifEnd", thisCnt);
        RPARENT();
        statement();
        if (thisSym == "ELSETK") {
            string endIf = generate_label_and_jump("endIf", thisCnt);  //直接生成j指令的四元式，跳到endIf
            set_label(ifEnd);  //生成相应四元式，不是直接print
            getSym(); statement();
            set_label(endIf);  //生成相应四元式，不是直接print
        } else {
            set_label(ifEnd);
        }
    } else
        error("in conditionStatement, not started with IFTK");
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<条件语句>\n");
}

string generate_label_and_jump(string labelName, int cnt) { //labelName = "", cnt = -1  Default
    string newName = "";
    if (cnt != -1)
        newName = labelName + "_" + to_string(cnt);
    pushQuaItemStack(new quaItem(newName, jumpType));
    generateQuadruple("jump", 1);
    return newName;
}

string jump_to_label(string labelName) {
    pushQuaItemStack(new quaItem(labelName, jumpType));
    generateQuadruple("jump", 1);
    return labelName;
}

string jal_to_label(string labelName) {
    pushQuaItemStack(new quaItem(labelName, jalType));
    generateQuadruple("jal", 1);
    return labelName;
}

string set_label(string basicString, int cnt) {
    string label = basicString;
    if (cnt != -1)
        label = label + "_" + to_string(cnt);
    pushQuaItemStack(new quaItem(label, labelType));
    generateQuadruple("set_label", 1);
    return label;
}

string condition(string labelHead, int cnt, int subIndex) {
    Type expressionType = expression();
    if (expressionType == charType)
        error("f", peekLine(-1));
    string opToken = relationOp();
    expressionType = expression();
    string endLabel = generate_label_then_branch(opToken, labelHead, cnt); //操作quaItemStack，生成branch四元式，返回ifEnd标签
    if (expressionType == charType)
        error("f", peekLine(-1));
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<条件>\n");
    return endLabel;
}

string generate_label_then_branch(string opString, string name, int thisCnt, int subCnt) { //opString should be the original opToken
    string instruction = "";
    if (opString == "LSS") {
        instruction = "bge";
    } else if (opString == "LEQ") {
        instruction = "bgt";
    } else if (opString == "GRE") {
        instruction = "ble";
    } else if (opString == "GEQ") {
        instruction = "blt";
    } else if (opString == "EQL") {
        instruction = "bne";
    } else if (opString == "NEQ") {
        instruction = "beq";
    }
    string label = name + "_" + to_string(thisCnt);
    if (subCnt != -1)
        label = label + "_" + to_string(subCnt);
    pushQuaItemStack(new quaItem(label, labelType));
    generateQuadruple(instruction, 3);
    return label;
}

void loopStatement() {
    if (thisSym == "WHILETK") {
        whileCnt++;
        int thisCnt = whileCnt;
        getSym();
        LPARENT();
        string whileHeadLabel = set_label("while", thisCnt);
        string endWhile = condition("endWhile", thisCnt);
        RPARENT();
        statement();
        jump_to_label(whileHeadLabel);
        set_label(endWhile);
    } else if (thisSym == "FORTK") { //记得处理步长，目前没有转换成中间代码
        forCnt++;
        int thisCnt = forCnt;
        getSym();
        LPARENT();
        tableItem *item = IDENFR(); //赋循环初值
        checkConstAssign(peekWord(-1));
        ASSIGN();
        pushQuaItemStack(new quaItem(item->codeName, item->type));
        expression();
        generateQuadruple("assign", 2);  //赋值结束
        SEMICN();
        string forHead = set_label("for", thisCnt);
        string endFor = condition("endFor", thisCnt);
        SEMICN();
        tableItem *itemLeft = IDENFR(); //步长更新还没做.   步长左值
        string leftName = itemLeft->codeName;
        Type leftType = itemLeft->type;
        checkConstAssign(peekWord(-1));
        ASSIGN();
        tableItem *itemRight = IDENFR(); //步长更新还没做.  步长右侧标识符
        string rightName = itemRight->codeName;
        Type rightType = itemRight->type;
        if (thisSym == "PLUS" || thisSym == "MINU") {
            string op = (thisSym == "PLUS") ? "+" : "-";
            getSym();
            int stepUInt = stepWidth();
            RPARENT();
            statement();
            //modify stepWidth
            pushQuaItemStack(new quaItem(leftName, leftType));
            pushQuaItemStack(new quaItem(rightName, rightType));
            pushQuaItemStack(new quaItem(to_string(stepUInt), constInt));
            generateQuadruple(op, 2);  //cal:op
            generateQuadruple("assign", 2);  //assign
            jump_to_label(forHead);
            set_label(endFor);
        } else
            error("in loopStatement, missing PLUS or MINU before stepWidth");
    } else
        error("in loopStatement, not started with WHILETK or FORTK");
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<循环语句>\n");
}

int stepWidth() {
    int u_int = INTCON();
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<步长>\n");
    return u_int;
}

void switchStatement() {
    if (thisSym == "SWITCHTK") {
        switchCnt++;
        int thisCnt = switchCnt;
        getSym();
        LPARENT();
        Type frontType = expression();
        RPARENT();
        LBRACE();
        string endCase = switchTable(frontType, thisCnt);
        switchDefault();
        RBRACE();
        set_label(endCase);
    } else
        error("in switchStatement, not started with ");
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<情况语句>\n");
}

string switchTable(Type frontType, int thisCnt) {
    int subCnt = 1;
    quaItem *expressionItem = topQuaItemStack();
    string endCase = subSwitch(frontType, thisCnt, subCnt, expressionItem);
    while (thisSym == "CASETK") {
        subCnt++;
        subSwitch(frontType, thisCnt, subCnt, expressionItem);
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<情况表>\n");
    return endCase;
}

string subSwitch(Type frontType, int thisCnt, int subCnt, quaItem *expressionItem) {
    string endCase = "";
    if (thisSym == "CASETK") {
        pushQuaItemStack(expressionItem); //赋值条件表达式的值
        getSym();
        string constValue = constVar(frontType);
        if (frontType == intType || frontType == constInt)
            pushQuaItemStack(new quaItem(constValue, constInt));
        else if (frontType == charType || frontType == constChar)
            pushQuaItemStack(new quaItem("\'" + constValue + "\'", constChar));
        string thisSubSwitchEnd = generate_label_then_branch("EQL", "subCaseEnd", thisCnt, subCnt);
        if (thisSym == "COLON") {
            getSym(); statement();
            endCase = generate_label_and_jump("endCase", thisCnt);
            set_label(thisSubSwitchEnd);
        } else
            error();
    } else
        error();
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<情况子语句>\n");
    return endCase;
}

void switchDefault() {
    if (thisSym == "DEFAULTTK") {
        getSym();
        if (thisSym == "COLON") {
            getSym(); statement();
        } else
            error();
    } else
        error("p", thisLine);
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<缺省>\n");
}

string relationOp() {
    string opToken = "";
    if (thisSym == "LSS" || thisSym == "LEQ" || thisSym == "GRE"
        || thisSym == "GEQ" || thisSym == "EQL" || thisSym == "NEQ") {
        opToken = thisSym;
        getSym();
    }
    else
        error();
    return opToken;
}

Type factor() {
    Type thisType = undefinedType;
    if (thisSym == "CHARCON") {
        thisType = charType;
        quaItem *thisItem = new quaItem("\'" + thisWord + "\'", constChar);
        pushQuaItemStack(thisItem);
        CHARCON();
    } else if (thisSym == "INTCON" || ((thisSym == "PLUS" || thisSym == "MINU") && peekSym(1) == "INTCON")) {
        thisType = intType;
        int num = INTEGER();
        quaItem *thisItem = new quaItem(to_string(num), constInt);
        pushQuaItemStack(thisItem);
    } else if (thisSym == "IDENFR" && peekSym(1) == "LPARENT") { //function with return
        string funcName = thisWord;
        //retFuncStatement();
        setItem(&searchItem, funcName, undefinedType, 4, false, true, level, 0);
        tablePt pt = find(searchItem);
        if (pt == &unfoundItem) {
            printf("in factor, previous retFunction register failed\n");
            thisType = intCharType;
        } else
            thisType = pt->type;
        retFuncStatement();
        pushQuaItemStack(new quaItem(funcName, thisType, 4));
        generateQuadruple("getReturnValue", 1, "", thisType);
    } else if (thisSym == "LPARENT") {
        LPARENT();
        expression();
        RPARENT(); //改type
        thisType = intType; //只要参与运算(此处运算为小括号)，就是int型，例如(‘c’)的结果是整型
		/*
        Type expQuaItemType = quaItemStack[quaItemStack.size() - 1]->getType();
        if (expQuaItemType == constChar)
            quaItemStack[quaItemStack.size() - 1]->setType(constInt);
        else if (expQuaItemType == charType)
            quaItemStack[quaItemStack.size() - 1]->setType(intType);
		*/
    } else if (thisSym == "IDENFR") {
        tablePt pt = IDENFR();
        string thisName = pt->codeName;
        if (pt == &unfoundItem) { //IDENFR not defined, error("c")emiteed in IDENFR();
            thisType = undefinedType;
        } else {
            thisType = pt->type;
        }
        if (thisSym == "LBRACK") { //a[5]
            Type indexType = undefinedType;
            LBRACK();
            indexType = expression(); //row_index
            if (indexType != intType)
                error("i", thisLine);
            RBRACK(); //a[(3+4)]
            if (thisSym == "LBRACK") {//a[(3+4)][5]
                LBRACK();
                indexType = expression();
                if (indexType != intType)
                    error("i", thisLine);
                generateQuadruple("useArray", 2, thisName, thisType);
                RBRACK();
            } else {
                pushQuaItemStack(placeHolderQuaItem);
                generateQuadruple("useArray", 2, thisName, thisType);
            }
        } else { // a, 不是数组
            quaItem *thisItem = new quaItem(thisName, thisType);
            pushQuaItemStack(thisItem);
        }
    } else
        error("in factor, started with wrong headings");
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<因子>\n");
    return thisType;
}

Type expression() {
    int cntOp = 0;
    bool minusFlag = false;
    if (thisSym == "PLUS" || thisSym == "MINU") {
        if (thisSym == "MINU") {
            pushQuaItemStack(new quaItem("0", constInt));
            minusFlag = true;
        }
        cntOp++;
        getSym();
    }
    Type thisItemType = item();
    if (minusFlag) //only if there's (-E), calculation's needed. Otherwise like (E), we may well keep E in quaItemStack
        generateQuadruple("-", 2);
    while (thisSym == "PLUS" || thisSym == "MINU") {
        string op = (thisSym == "PLUS") ? "+" : "-";
        cntOp++;
        getSym();
        item();
        generateQuadruple(op, 2);
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<表达式>\n");

    if (cntOp == 0 && thisItemType == charType) {
        return charType;
    } else {
        Type expQuaItemType = quaItemStack[quaItemStack.size() - 1]->getType();
        if (expQuaItemType == constChar)
            quaItemStack[quaItemStack.size() - 1]->setType(constInt);
        else if (expQuaItemType == charType)
            quaItemStack[quaItemStack.size() - 1]->setType(intType);
        return intType;
    }
}

Type item() {
    bool hasOp = false;
    Type factorType = factor();
    while (thisSym == "MULT" || thisSym == "DIV") {
        string op = (thisSym == "MULT") ? "*" : "/";
        hasOp = true;
        getSym();
        factor();
        generateQuadruple(op, 2);
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<项>\n");

    if (hasOp == false  && factorType == charType)
        return charType;
    else
        return intType;
}

void retFuncStatement() {
    string thisFuncName = IDENFR()->codeName;
    LPARENT();
    vector<Type> definedTypeVector = getRetFuncDefinedTypeVector(thisFuncName);
    pushQuaItemStack(new quaItem(thisFuncName, funcType, 0)); //funcCallHeadMark
    generateQuadruple("funcCallHeadMark", 1);
    inputFuncTable(retFuncNameMap[thisFuncName], false, thisFuncName);
    RPARENT();
    //init
    pushQuaItemStack(new quaItem(thisFuncName, funcType, 0)); //move $sp
    generateQuadruple("retFuncCall", 1);
    jal_to_label(thisFuncName);
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<有返回值函数调用语句>\n");
}

void noRetFuncStatement() { //保证进来的都是定义过的函数名
    string thisFuncName = IDENFR()->codeName;
    LPARENT();
    vector<Type> definedTypeVector = getNoRetFuncDefinedTypeVector(thisFuncName);
    pushQuaItemStack(new quaItem(thisFuncName, funcType, 0)); //funcCallHeadMark
    generateQuadruple("funcCallHeadMark", 1);
    inputFuncTable(definedTypeVector, false, thisFuncName);
    RPARENT();
    //init
    pushQuaItemStack(new quaItem(thisFuncName, funcType, 0)); // move $sp
    generateQuadruple("noRetFuncCall", 1);
    jal_to_label(thisFuncName);
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<无返回值函数调用语句>\n");
}

vector<Type> getNoRetFuncDefinedTypeVector(string funcName) {
    string str = funcName;
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return noRetFuncNameMap[str];
}

vector<Type> getRetFuncDefinedTypeVector(string funcName) {
    string str = funcName;
    transform(str.begin(), str.end(), str.begin(), ::tolower);
    return retFuncNameMap[str];
}

void undefinedFunction() {
    IDENFR();
    LPARENT();
    vector<Type> emptyVec;
    inputFuncTable(emptyVec, true);
    RPARENT();
}

void inputFuncTable(vector<Type> defineTypeVector, bool isUndefined, string funcName) {
    vector<Type> usedTypeVector;
    Type thisParaType = undefinedType;
    int offset = 8;
    if (thisSym == "RPARENT" || thisSym == "SEMICN") //为空,同时还要考虑右括号缺失的问题
        ; //do not getSym()
    else {
        thisParaType = expression();
        usedTypeVector.push_back(thisParaType);
        pushQuaItemStack(new quaItem(to_string(offset), constInt, 4));
        generateQuadruple("paraInFuncCall", 2, funcName);
    }
    while (thisSym == "COMMA") {
        offset += 4;
        getSym();
        thisParaType = expression();
        usedTypeVector.push_back(thisParaType);
        pushQuaItemStack(new quaItem(to_string(offset), constInt, 4));
        generateQuadruple("paraInFuncCall", 2, funcName);
    }
    if (!isUndefined) { // if defined, then get in
        if (defineTypeVector.size() != usedTypeVector.size())
            error("d", thisLine); //报参数表右括号
        else {
            for (int i = 0; i < defineTypeVector.size(); i++)
                if (defineTypeVector[i] != usedTypeVector[i])
                    error("e", thisLine);
        }
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<值参数表>\n");
}


void printfStatement() {
    if (thisSym == "PRINTFTK") {
        getSym();
        LPARENT();
        if (thisSym != "STRCON") { //printf '(' <表达式>
            expression(); //thisSym = ')'
            generateQuadruple("printf", 1);
        } else { //printf '(' <字符串>
            quaItem *stringQuaItem = new quaItem("\"" + thisWord + "\"", constString);
            pushQuaItemStack(stringQuaItem);
            STRCON();
            if (thisSym == "COMMA") { //printf '(' <字符串>,<表达式> ')'
                getSym();
                expression();
                generateQuadruple("printf", 2);
            } else {
                generateQuadruple("printf", 1);
            }
        }
        RPARENT();
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<写语句>\n");
}

void scanfStatement() {
    tableItem *ptr = nullptr;
    if (thisSym == "SCANFTK") {
        getSym();
        LPARENT();
        ptr = IDENFR();
        checkConstAssign(peekWord(-1));
        RPARENT();
    } else
        error();

    pushQuaItemStack(new quaItem(ptr->codeName, ptr->type));
    generateQuadruple("scanf", 1);
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<读语句>\n");
}

void checkConstAssign(string identifierName) {
    setItem(&searchItem, identifierName, undefinedType, 4, false, false, level, 0);
    tablePt pt = find(searchItem);
    if (pt != NULL && pt->isConst == true)
        error("j", peekLine(-1));
}

void returnStatement() {
    if (thisSym == "RETURNTK") {
        returnNum++;
        getSym();
        if (funcRetType == voidType) { //void func()
            if (thisSym == "LPARENT") { //一行只会出现一个错误，如果发现左括号，立即报错，后续按照(<表达式>)分析
                error("g", thisLine);
                LPARENT();
                expression();
                RPARENT();
            } else
                ; //若是少了分号，必然是缺少分号的错误
            if (inMainFunc) {
                generateQuadruple("mainReturn", 0);
            } else {
                generateQuadruple("noRetFuncReturn", 0);
            }
        } else if (funcRetType == intType ||funcRetType == charType) {
            if (thisSym == "SEMICN") { //return ;
                error("h", thisLine); //不需要getSym。返回值语句里不包括分号
            } else {
                LPARENT();
                if (thisSym == "RPARENT") { //return()
                    error("h", thisLine);
                    RPARENT();
                } else { //return'('<expresion>
                    Type expressionType = expression();
                    pushQuaItemStack(new quaItem("returnType", funcRetType, 0));
                    if (expressionType != funcRetType)
                        error("h", thisLine);
                    RPARENT(); //return'('<expresion>')'
                    generateQuadruple("retFuncReturn", 2);
                }
            }
        }
    } else
        error();
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<返回语句>\n");
}

vector<Type> funcTable(vector<Type> paraTable) {
    int offset = 8;
    string name = "";
    Type thisParaType = undefinedType;
    if (thisSym == "INTTK" || thisSym == "CHARTK") {
        thisParaType = (thisSym == "INTTK") ? intType : charType;
        getSym();
        name = IDENFR(true)->codeName;
        registerItem(name, thisParaType, 4, false, false, level + 1, 0); //在函数里用
        paraTable.push_back(thisParaType);
        pushQuaItemStack(new quaItem(name, thisParaType, 4));
        pushQuaItemStack(new quaItem(to_string(offset), constInt, 4));
        generateQuadruple("paraInFuncDef", 2);
        while (thisSym == "COMMA") {
            offset += 4;
            getSym();
            thisParaType = typeSymbol();
            name = IDENFR(true)->codeName; //IDENFN will get the nextSym
            registerItem(name, thisParaType, 4, false, false, level + 1, 0); //在函数里用
            paraTable.push_back(thisParaType);
            pushQuaItemStack(new quaItem(name, thisParaType, 4));
            pushQuaItemStack(new quaItem(to_string(offset), constInt, 4));
            generateQuadruple("paraInFuncDef", 2);
        }
    } //else do nothing. Because if funcTable is empty, thisSym should already be ')'
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<参数表>\n");
    return paraTable;
}

Type typeSymbol() {
    Type thisType = undefinedType;
    if (thisSym == "INTTK" || thisSym == "CHARTK") {
        thisType = (thisSym == "INTTK") ? intType : charType;
        getSym();
    } else
        error("missing typeS of int or char");
    return thisType;
}

void mainFunction() {
    if (thisSym == "VOIDTK") getSym();
    else error();
    if (thisSym == "MAINTK") getSym();
    else error();
    generateQuadruple("mainFunc", 0);
    LPARENT();
    RPARENT();
    LBRACE();
    level++;
    funcRetType = voidType;
    compoundStatement();
    funcRetType = undefinedType;
    level--;
    RBRACE();
    set_label("mainEnd");
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<主函数>\n");
}

void plusAndMinus() //no output
{
    if (thisSym == "PLUS" || thisSym == "MINU") {
        getSym();
    } else {
        error();
    }
}
