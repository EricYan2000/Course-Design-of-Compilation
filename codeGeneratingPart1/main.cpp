#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <set>
#include "wordDealing.h"
#include "type.h"
#include "symbolTable.h"
#include "quadruple.h"
#include "memMap.h"

using namespace std;

FILE *grammarDealingOUT = fopen(grammarDealingOutput, "w");
FILE *errorOUT = fopen(errorDealingOutput, "w");
FILE *mipsOUT = fopen(mipsOutput, "w");

unordered_map<string, vector<Type>> retFuncNameMap;
unordered_map<string, vector<Type>> noRetFuncNameMap;

//symbolTable operations
int registerItem(string name, Type type, bool isConst, bool isFunc, int level, int dimension);
void popTable();


//basic information
int symIndex = -1;
int grammarDealing();
int getSym();  //get the next Sym from codeMap
string peekSym(int i);  //get number i element behind codeMap[symIndex]
string peekWord(int i);
int peekLine(int i);
void error(string errorCode = "", int line = 0, string info = "");
int level = 0;
Type funcRetType = undefinedType;   //widely used don't delete
int returnNum = 0;                  //widely used don't delete

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
void condition();
void relationOp();
void loopStatement();
void stepWidth();
void switchStatement();
void switchTable(Type frontType);
void switchDefault();
void subSwitch(Type frontType);
void inputFuncTable(vector<Type> defineTypeVector, bool isUndefined = false);
bool searchForAssign();
void checkConstAssign(string identifierName);
void undefinedFunction();

// function's paraTable debug
void printParaTable();

// form quadruple
vector<quaItem*> quaItemStack;
void pushQuaItemStack(quaItem* quaItemPtr);
quaItem* popStack();
vector<quadruple*> quadrupleStack;
void pushQuadrupleStack(quadruple* quadruplePtr);
void printQuaItemStack();
void generateQuadruple(string op, int quaItemNeeded);
void printBridgeCode();
void generateDefineQuadruple(defineType type, string name, string value);
void generateMipsCode();
void generateDotData();
void appendDotData(quaItem *pItem);
//string assignSpace(quadruple *ptr);
static int msgCnt = 0;
//static int stackTop = 0x10040000;
int quadruple::quadrupleCnt = 0;

//unordered_map<string, string> memMapping;
//string getAddress(quadruple *ptr);

//void printMapping();

int main()
{
    generateDotData();

    wordDealing();

    grammarDealing();   //getSym() in its first line

    printQuaItemStack();

    printBridgeCode();

    printf("[WARINING!!] 现在的内存分配以及mapping没有考虑level不同的情况\n");

    generateMipsCode();

    printMapping();

    return 0;
}

//generate mips code
void generateMipsCode() {
    printf("[generating mips code]\n");
    fprintf(mipsOUT, "\n.text\n");
    for (int i = 0; i < quadrupleStack.size(); i++)
        quadrupleStack[i]->generateCode(mipsOUT);
    printf("[mips code done!] Don't forget to redirect the mips code to mips.txt\n");
    return ;
}

void generateDotData() {
    fprintf(mipsOUT, ".data\n");
    fprintf(mipsOUT, "\tenter: .asciiz \"\\n\"\n");
}


void appendDotData(quaItem *quaItemPtr) {
    msgCnt++;
    string info= "\tmsg" + to_string(msgCnt) + ": .asciiz " + quaItemPtr->getName() + "\n";
    fprintf(mipsOUT, info.c_str());
    memMapping[quaItemPtr->getName()] = "msg" + to_string(msgCnt);
}
/*
string assignSpace(quadruple *ptr) {
    stackTop = stackTop - 4;
    memMapping[ptr->getName()] = to_string(stackTop);
    return to_string(stackTop);
}

string getAddress(quadruple *ptr) {
    int cnt = memMapping.count(ptr->getName());
    if (cnt != 0)
        return memMapping[ptr->getName()];
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
*/

// form quadruple
void pushQuaItemStack(quaItem* quaItemPtr) {
    quaItemStack.push_back(quaItemPtr);
    if (quaItemPtr->getType() == constString)
        appendDotData(quaItemPtr);
}

quaItem* popStack() {
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

void pushQuadrupleStack(quadruple* quadruplePtr) {
    quadrupleStack.push_back(quadruplePtr);
    quadrupleKind thisKind = quadruplePtr->getKind();
    if (thisKind == constIntDefinition || thisKind == constCharDefinition ||
        thisKind == varIntDefinition || thisKind == varCharDefinition)
        assignSpace(quadruplePtr);
    else if (thisKind == calculateKind)
        assignSpace(quadruplePtr);
}

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

void generateQuadruple(string op, int quaItemNeeded) {
    int length = quaItemStack.size();
    if (length < quaItemNeeded)
        printf("stack here fewer quaItem then needed\n");
    else {
        quadruple *quadruplePtr = nullptr;
        if (quaItemNeeded == 2) {
            quaItem *item2 = popStack();
            quaItem *item1 = popStack();
            quadruplePtr = new quadruple(op, item1, item2); //do not switch sequence
        } else if (quaItemNeeded == 1) {
            quadruplePtr = new quadruple(op, popStack(), nullptr); //do not switch sequence
        } else {
            printf("quaItemNeeded number undefined, in generateQuadruple\n");
        }
        pushQuadrupleStack(quadruplePtr);
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
void error(string errorCode, int line, string info) { //default missing 没有用这个函数
    int length = errorCode.length();
    if (length == 1) {
        fprintf(errorOUT, "%d %s\n", line, errorCode.c_str());
    } else {
        //fprintf(grammarDealingOUT, errorCode.c_str());
        printf("%s\n", errorCode.c_str());
    }
}

int grammarDealing()
{
    getSym();
    program();
    if (paraTableDebug) {
        printParaTable();
    }
    return 0;
}

void printParaTable() {
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

//table starts from here

int registerItem(string name, Type type, bool isConst, bool isFunc, int level, int dimension) {
    setItem(&insertItem, name, type, isConst, isFunc, level, dimension);
    tablePt found = find(insertItem);
    if (found == NULL || found->level < level) {
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

void popTable() {
    bool flag = true;
    while (table.size() != 0 && flag) {
        if (table[table.size() - 1].level > level) {
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
}

//grammar dealing functions start from here

int getSym() //thisSym判断才是真正的读，因此getSym的实质还是预读。因此，在getSym下一个时，输出上一个getSym
{
    symIndex++;
    if (symIndex <= vectorSize) {
        if (symIndex < vectorSize) {
            thisSym = codeVec[symIndex];
            thisWord = wordVec[symIndex];
            thisLine = lineVec[symIndex];
        }
        if (grammarDealingDebug && symIndex >= 1)
            fprintf(grammarDealingOUT, "%s %s\n", codeVec[symIndex - 1].c_str(), wordVec[symIndex - 1].c_str());
        return 1;
    } else {
        thisSym = "";
        return 0;
    }
}

string peekSym(int i)
{
    if (symIndex + i < vectorSize)
        return codeVec[symIndex + i];
    else
        return "";
}

string peekWord(int i)
{
    if (symIndex + i < vectorSize)
        return wordVec[symIndex + i];
    else
        return "";
}

int peekLine(int i)
{
    if (symIndex + i <= 0)
        return 0;
    else if (symIndex + i >= vectorSize)
        return thisLine + 1;
    else
        return lineVec[symIndex +i];
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
    level++;
}

void RBRACE() {
    //see noRetFunc() and retFunc()
    if (braceDebug)
        printf("line %d should have RBRACE\n", thisLine);
    if (thisSym == "RBRACE") {
        getSym();
    } else {
        error("missing RBRACE");
    }
    level--;
    popTable();
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
    while((thisSym == "INTTK" || thisSym == "CHARTK" || thisSym == "VOIDTK")
          && peekSym(2) == "LPARENT" && peekSym(1) == "IDENFR") { //functions  // &&peekSym(1) == "IDENFR"
        functions();
    }
    mainFunction();
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
        registerItem(name, thisType, true, false, level, 0);
        ASSIGN();
        int value = INTEGER();
        generateDefineQuadruple(defineConstInt, name, to_string(value));
        while (thisSym == "COMMA") {
            getSym();
            name = IDENFR(true)->codeName;
            registerItem(name, thisType, true, false, level, 0);
            ASSIGN();
            value = INTEGER();
            generateDefineQuadruple(defineConstInt, name, to_string(value));
        }
    } else if (thisSym == "CHARTK") { //char c = 'a', p = 'z'
        Type thisType = charType;
        getSym();
        name = IDENFR(true)->codeName;
        registerItem(name, thisType, true, false, level, 0);
        ASSIGN();
        string value = CHARCON();
        generateDefineQuadruple(defineConstChar, name, value);
        while (thisSym == "COMMA") {
            COMMA();
            name = IDENFR(true)->codeName;
            registerItem(name, thisType, true, false, level, 0);
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
            generateQuadruple("defineVarInt", 2);
        } else {
            pushQuaItemStack(new quaItem(name, charType));
            generateQuadruple("defineVarInt", 1);
        }
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
        registerItem(name, thisType, false, false, level, 0);
        ASSIGN();
        value = constVar(thisType);   //end of the zero-dimension, like int a = 10;
        defineType thisDefineType = (thisType == intType) ? defineVarInt : defineVarChar;
        generateDefineQuadruple(thisDefineType, name, value);
    } else { //int a[
        LBRACK();
        int row = INTCON();
        RBRACK(); //int a[5]
        if (thisSym == "ASSIGN") { //int a[5] =
            registerItem(name, thisType, false, false, level, 1);
            ASSIGN();
            LBRACE(); //int a[5] = {
            constVar(thisType);
            int cntRow = 1;
            while (thisSym == "COMMA") {
                cntRow++;
                getSym(); constVar(thisType);
            }
            if (cntRow != row)
                error("n", thisLine);
            RBRACE(); //int a[5] = {1,2}
        } else { //int a[5][
            LBRACK();
            int column = INTCON();
            RBRACK(); //int a[5][7]
            ASSIGN();
            registerItem(name, thisType, false, false, level, 2);
            assignTwoDimensionWithConstVar(thisType, row, column); //ends with getSym()
        }
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<变量定义及初始化>\n");
}

void assignTwoDimensionWithConstVar(Type thisType, int row, int column) { //int a[5][7] =  //used in presetVarDefinition
    int cntRow = 0;
    int cntColumn = 0;
    LBRACE();
    LBRACE(); //int a[5][7] = {{
    cntRow++;
    constVar(thisType);
    cntColumn++;
    while (thisSym == "COMMA") {
        getSym(); constVar(thisType);
        cntColumn++;
    }
    if (cntColumn != column)
        error("n", thisLine);
    RBRACE();
    while (thisSym == "COMMA") { //int a[5][7] = {{1,2} ,
        cntColumn = 0;
        getSym();
        LBRACE(); //int a[5][7] = {{1,2} , {
        cntRow++;
        constVar(thisType);
        cntColumn++;
        while (thisSym == "COMMA") { //int a[5][7] = {{1,2} , {22,3,
            getSym(); constVar(thisType);
            cntColumn++;
        }
        if (cntColumn != column)
            error("n", thisLine);
        RBRACE(); //int a[5][7] = {{1,2} , {22,3,4}
    }
    RBRACE(); //should be int a[5][7] = {{1,2} , {22,3,4}}
    if (cntRow != row)
        error("n", peekLine(-1));
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
        INTCON();
        RBRACK(); //end of the one-dimension, like c[10]
        if (thisSym == "LBRACK") { //two-dimension. like char c[5][10]
            registerItem(name, thisType, false, false, level, 2);
            LBRACK();
            INTCON();
            RBRACK();
        } else { //one-dimension. like char c[10]
            registerItem(name, thisType, false, false, level, 1);
        }
    } else { //zero-dimension. like char c,
        registerItem(name, thisType, false, false, level, 0);
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
    setItem(&searchItem, thisWord, undefinedType, false, false, level, 0);
    if (!isDeclare) { //check whether declared
        pt = find(searchItem);
        if (pt == NULL)
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
    int cnt = noRetFuncNameMap.count(funcName);  // != 0 means, there's already a legal function calls funcName
    vector<Type> paraTable;
    if (cnt == 0)
        addNoRetFunc(funcName, paraTable); //嵌套问题，不可以换地方
    if (thisSym == "VOIDTK") {
        getSym();
        IDENFR(true);
        registerItem(funcName, voidType, false, true, level, 0);
        LPARENT();
        paraTable = funcTable(paraTable);
        if (cnt == 0)
            addNoRetFunc(funcName, paraTable); //嵌套问题，不可以换地方
        RPARENT();
        LBRACE();
        compoundStatement();
        RBRACE();
    } else {
        error();
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<无返回值函数定义>\n");
}

void retFunc() {
    string funcName = peekWord(1);
    int cnt = retFuncNameMap.count(funcName);  // != 0 means, there's already a legal function calls funcName
    //check 有没有重复
    vector<Type> paraTable;
    if (cnt == 0)
        addRetFunc(funcName, paraTable); //解决fib嵌套定义的问题，不可以换地方
    int errorFlag = 0;
    Type thisFuncType = funcHead();
    registerItem(funcName, thisFuncType, false, true, level, 0);
    LPARENT();
    paraTable = funcTable(paraTable);
    if (cnt == 0)
        addRetFunc(funcName, paraTable); //解决fib嵌套定义的问题，不可以换地方
    RPARENT();
    LBRACE();
    compoundStatement();
    RBRACE();
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
    tableItem *item = IDENFR();
    checkConstAssign(peekWord(-1));
    if (thisSym == "ASSIGN") { //a = 1 + 2;
        ASSIGN();
        expression();
        pushQuaItemStack(new quaItem(item->codeName, item->type));
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
            expression();
        } else { //a[5][
            LBRACK();
            indexType = expression();
            if (indexType != intType)
                error("i", thisLine);
            RBRACK(); //a[5][6]
            ASSIGN();
            expression();
        }
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<赋值语句>\n");
}

void conditionStatement() {
    if (thisSym == "IFTK") {
        getSym();
        LPARENT();
        condition();
        RPARENT();
        statement();
        if (thisSym == "ELSETK") {
            getSym(); statement();
        }
    } else
        error("in conditionStatement, not started with IFTK");
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<条件语句>\n");
}

void condition() {
    Type expressionType = expression();
    if (expressionType == charType)
        error("f", peekLine(-1));
    relationOp();
    expressionType = expression();
    if (expressionType == charType)
        error("f", peekLine(-1));
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<条件>\n");
}

void loopStatement() {
    if (thisSym == "WHILETK") {
        getSym();
        LPARENT();
        condition();
        RPARENT();
        statement();
    } else if (thisSym == "FORTK") {
        getSym();
        LPARENT();
        IDENFR();
        checkConstAssign(peekWord(-1));
        ASSIGN();
        expression();
        SEMICN();
        condition();
        SEMICN();
        IDENFR();
        checkConstAssign(peekWord(-1));
        ASSIGN();
        IDENFR();
        if (thisSym == "PLUS" || thisSym == "MINU") {
            getSym(); stepWidth();
            RPARENT();
            statement();
        } else
            error("in loopStatement, missing PLUS or MINU before stepWidth");
    } else
        error("in loopStatement, not started with WHILETK or FORTK");
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<循环语句>\n");
}

void stepWidth() {
    INTCON();
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<步长>\n");
}

void switchStatement() {
    if (thisSym == "SWITCHTK") {
        getSym();
        LPARENT();
        Type frontType = expression();
        RPARENT();
        LBRACE();
        switchTable(frontType);
        switchDefault();
        RBRACE();
    } else
        error("in switchStatement, not started with ");
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<情况语句>\n");
}

void switchTable(Type frontType) {
    subSwitch(frontType);
    while (thisSym == "CASETK") {
        subSwitch(frontType);
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<情况表>\n");
}

void subSwitch(Type frontType) {
    if (thisSym == "CASETK") {
        getSym();
        constVar(frontType);
        if (thisSym == "COLON") {
            getSym(); statement();
        } else
            error();
    } else
        error();
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<情况子语句>\n");
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

void relationOp() {
    if (thisSym == "LSS" || thisSym == "LEQ" || thisSym == "GRE"
        || thisSym == "GEQ" || thisSym == "EQL" || thisSym == "NEQ" )
        getSym();
    else
        error();
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
    } else if (thisSym == "IDENFR" && peekSym(1) == "LPARENT") {
        string funcName = thisWord;
        retFuncStatement();
        setItem(&searchItem, funcName, undefinedType, false, true, level, 0);
        tablePt pt = find(searchItem);
        if (pt == NULL) {
            printf("in factor, previous retFunction register failed\n");
            thisType = intCharType;
        } else
            thisType = pt->type;
    } else if (thisSym == "LPARENT") {
        LPARENT();
        expression();
        RPARENT(); //改type
        thisType = intType; //只要参与运算，就是int型，例如(‘c’)的结果是整型
		/*
        Type expQuaItemType = quaItemStack[quaItemStack.size() - 1]->getType();
        if (expQuaItemType == constChar)
            quaItemStack[quaItemStack.size() - 1]->setType(constInt);
        else if (expQuaItemType == charType)
            quaItemStack[quaItemStack.size() - 1]->setType(intType);
		*/
    } else if (thisSym == "IDENFR") {
        tablePt pt = IDENFR();
        if (pt == NULL) { //IDENFR not defined, error("c")emiteed in IDENFR();
            thisType = undefinedType;
        } else {
            thisType = pt->type;
            quaItem *thisItem = new quaItem(pt->codeName, thisType);
            pushQuaItemStack(thisItem);
        }
        if (thisSym == "LBRACK") {
            Type indexType = undefinedType;
            LBRACK();
            indexType = expression();
            if (indexType != intType)
                error("i", thisLine);
            RBRACK(); //a[(3+4)]
            if (thisSym == "LBRACK") {
                LBRACK();
                indexType = expression();
                if (indexType != intType)
                    error("i", thisLine);
                RBRACK(); //a[(3+4)][5]
            }
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

    if (cntOp == 0 && thisItemType == charType)
        return charType;
    else {
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
    inputFuncTable(retFuncNameMap[thisFuncName]);
    RPARENT();
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<有返回值函数调用语句>\n");
}

void noRetFuncStatement() { //保证进来的都是定义过的函数名
    string thisFuncName = IDENFR()->codeName;
    LPARENT();
    vector<Type> definedTypeVector = getNoRetFuncDefinedTypeVector(thisFuncName);
    inputFuncTable(definedTypeVector);
    RPARENT();
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

void inputFuncTable(vector<Type> defineTypeVector, bool isUndefined) {
    vector<Type> usedTypeVector;
    if (thisSym == "RPARENT" || thisSym == "SEMICN") //为空,同时还要考虑右括号缺失的问题
        ; //do not getSym()
    else
        usedTypeVector.push_back(expression());
    while (thisSym == "COMMA") {
        getSym();
        usedTypeVector.push_back(expression());
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
    setItem(&searchItem, identifierName, undefinedType, false, false, level, 0);
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
                    if (expressionType != funcRetType)
                        error("h", thisLine);
                    RPARENT(); //return'('<expresion>')'
                }
            }
        }
    } else
        error();
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<返回语句>\n");
}

vector<Type> funcTable(vector<Type> paraTable) {
    string name = "";
    Type thisParaType = undefinedType;
    if (thisSym == "INTTK" || thisSym == "CHARTK") {
        thisParaType = (thisSym == "INTTK") ? intType : charType;
        getSym();
        name = IDENFR(true)->codeName;
        registerItem(name, thisParaType, false, false, level + 1, 0); //在函数里用
        paraTable.push_back(thisParaType);
        while (thisSym == "COMMA") {
            getSym();
            thisParaType = typeSymbol();
            name = IDENFR(true)->codeName; //IDENFN will get the nextSym
            registerItem(name, thisParaType, false, false, level + 1, 0); //在函数里用
            paraTable.push_back(thisParaType);
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
    LPARENT();
    RPARENT();
    LBRACE();
    funcRetType = voidType;
    compoundStatement();
    funcRetType = undefinedType;
    RBRACE();
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
