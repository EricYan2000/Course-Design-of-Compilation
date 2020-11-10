#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <set>

using namespace std;

enum Type {undefinedType, intType, charType, voidType, funcType, intCharType};

typedef struct tableItem {
    string codeName;
    Type type;
    bool isConst;
    bool isFunc;
    int level;
    int dimension;
    bool operator==(const tableItem b) const
    {
        return this->codeName == b.codeName;
    }
}tableItem, *tablePt;

bool readInDebug = false; //print the original code it has read
bool grammarDealingDebug = true;
bool tableDebug = true;
bool braceDebug = false;
bool paraTableDebug = true;

FILE *IN = fopen("testfile.txt", "r");
FILE *wordDealingOUT = fopen("wordDealing.txt", "w");
FILE *grammarDealingOUT = fopen("grammarDealing.txt", "w");
FILE *errorOUT = fopen("error.txt", "w");
#define MAXLEN 65536
#define TEMPLEN 8192
string thisSym = "";    //store the result of getSym(codeMap[symIndex]) in thisSym
string thisWord = "";   //store the text of the word
int thisLine = 0;       //store the line number of the current word
char in_line[MAXLEN + 5] = {0};
char temp[TEMPLEN + 5] = {0};
char charToken[3] = {'\0', '\0', '\0'};
int line = 0;
int col = 0;
int vectorSize = 0; //size of wordMap and codeMap, represents the number of words read in
int symIndex = -1;  //used in nextSym to return codeMap[symIndex]. Added by 1 by getSym when every time used
vector<string> wordVec;
vector<string> codeVec;
vector<int> lineVec;
unordered_map<char, int> bracketMap {
    {'(', 1},
    {')', 2},
    {'[', 3},
    {']', 4},
    {'{', 5},
    {'}', 6}
};
unordered_map<string, string> codeMap {
    {"const", "CONSTTK"}, {"int", "INTTK"}, {"char", "CHARTK"}, {"void", "VOIDTK"},
    {"main", "MAINTK"}, {"if", "IFTK"}, {"else", "ELSETK"}, {"switch", "SWITCHTK"},
    {"case", "CASETK"}, {"default", "DEFAULTTK"}, {"while", "WHILETK"}, {"for", "FORTK"},
    {"scanf", "SCANFTK"}, {"printf", "PRINTFTK"}, {"return", "RETURNTK"},
    {"+", "PLUS"}, {"-", "MINU"}, {"*", "MULT"}, {"/", "DIV"},
    {"<", "LSS"}, {"<=", "LEQ"}, {">", "GRE"}, {">=", "GEQ"},
    {"==", "EQL"}, {"!=", "NEQ"}, {"=", "ASSIGN"},
    {":", "COLON"}, {";", "SEMICN"}, {",", "COMMA"}, {"(", "LPARENT"}, {")", "RPARENT"},
    {"[", "LBRACK"}, {"]", "RBRACK"}, {"{", "LBRACE"}, {"}", "RBRACE"},
};
unordered_map<string, vector<Type>> retFuncNameMap;
unordered_map<string, vector<Type>> noRetFuncNameMap;
//set<string> retFuncNameSet;
//set<string> noRetFuncNameSet;

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

//table operations
int registerItem(string name, Type type, bool isConst, bool isFunc, int level, int dimension);
void setItem(tablePt pt, string name, Type type, bool isConst, bool isFunc, int level, int dimension);
tablePt find(tableItem target);
void popTable();
void printItem(tableItem item);

//basic information
int grammarDealing();
int getSym();  //get the next Sym from codeMap
string peekSym(int i);  //get number i element behind codeMap[symIndex]
string peekWord(int i);
int peekLine(int i);
void error(string errorCode = "", int line = 0, string info = "");
int missingDecideErrorLine();
int level = 0;
Type funcRetType = undefinedType; //widely used
int returnNum = 0;

//wordDealing
int wordDealing();
int isAlpha(char c);
int isBrackets(char c);
int isRelation(char c);
char* processNum();
char* processIdentifier();
char* processCharToken();
char* processString();
char* processReation();
void storeObj(char *obj, int mode, string code, string word);
void printSingleChar(char c);
void addToVec(string code, string word);    //store words and codes in to wordVec and codeVec
void printVec();    //print words and codes to wordDealingOutput.txt

//grammarDealing
void program();
void constDeclaration();
void varDeclaration();
void functions();
void mainFunction();
void constDefinition();
tablePt IDENFR(bool isDeclare = false);
int INTCON();
void CHARCON();
void STRCON();
bool validStr(string basicString);
void INTEGER();
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
void constVar(Type frontType = intCharType);
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

void printParaTable();
void checkConstAssign(string identifierName);
void undefinedFunction();

int main()
{
    wordDealing();

    grammarDealing();   //getSym() in its first line

    return 0;
}

//basic functions
void error(string errorCode, int line, string info) { //default missing 没有用这个函数
    int length = errorCode.length();
    if (length == 1) {
        fprintf(errorOUT, "%d %s\n", line, errorCode.c_str());
    } else {
        fprintf(grammarDealingOUT, info.c_str());
    }
}

int missingDecideErrorLine() { //default missing 没有用这个函数
    return peekLine(-1);
}

int wordDealing()
{
    char *identifier = NULL, *num = NULL;
    //memset(in_line, 0, MAXLEN);
    int bras = 0;
    char c = 0;
    int i = 0;
    //while (fgets(in_line, MAXLEN, IN) != NULL) {
    while (c != EOF) {
        memset(in_line, 0, MAXLEN);
        i = 0;
        while ((c = fgetc(IN)) != EOF) {
            if(readInDebug)
                printf("%c", c);
            in_line[i] = c;
            i++;
            if (c == '\n')
                break;
        }

        if (c == EOF) {
            in_line[i] = '\n';
        }

        line++;
        col = 0;
        while (in_line[col] != '\n' && in_line[col] != EOF) { //start dealing
            if (isAlpha(in_line[col])) {
                char* identifier = processIdentifier();
                storeObj(identifier, 0, "", "");
            } else if (isdigit(in_line[col])) {
                char* num = processNum(); //col is added here
                storeObj(num, 1, "INTCON", num);
            } else if (in_line[col] == '\'') {
                char* curChar = processCharToken();
                char word[2] = {0, 0};
                word[0] = *curChar;
                storeObj(curChar, 1, "CHARCON", word);
            } else if (in_line[col] == '\"') {
                processString();
                storeObj(temp, 1, "STRCON", temp);
            } else if (bras = isBrackets(in_line[col])) {
                printSingleChar(in_line[col]);
                col++;
            } else if (in_line[col] == ';' || in_line[col] == ',' ||
                       in_line[col] == '-' || in_line[col] == '+' || in_line[col] == ':') {
                printSingleChar(in_line[col]);
                col++;
            } else if (in_line[col] == '*') {
                printSingleChar(in_line[col]);
                col++;
            } else if (in_line[col] == '/') {
                printSingleChar(in_line[col]);
                col++;
            } else if (isRelation(in_line[col])) {
                char* relation = processReation();
                storeObj(relation, 0, "", "");
            } else {
                col++;
            }
        }
    }

    printVec(); //to wordDealingOutput.txt
    vectorSize = wordVec.size();
    fclose(IN);
    fclose(wordDealingOUT);
    return 0;
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
        error("k", missingDecideErrorLine());
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
        error("l", missingDecideErrorLine());
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
        error("m", missingDecideErrorLine());
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
        INTEGER();
        while (thisSym == "COMMA") {
            getSym();
            name = IDENFR(true)->codeName;
            registerItem(name, thisType, true, false, level, 0);
            ASSIGN();
            INTEGER();
        }
    } else if (thisSym == "CHARTK") { //char c = 'a', p = 'z'
        Type thisType = charType;
        getSym();
        name = IDENFR(true)->codeName;
        registerItem(name, thisType, true, false, level, 0);
        ASSIGN();
        CHARCON();
        while (thisSym == "COMMA") {
            COMMA();
            name = IDENFR(true)->codeName;
            registerItem(name, thisType, true, false, level, 0);
            ASSIGN();
            CHARCON();
        }
    } else
        error();
    if (grammarDealingDebug && thisSym != "COMMA")
        fprintf(grammarDealingOUT, "<常量定义>\n");
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
    Type thisType = typeSymbol(); //getSym() and error() inside
    name = IDENFR(true)->codeName;
    if (thisSym == "ASSIGN") {
        registerItem(name, thisType, false, false, level, 0);
        ASSIGN();
        constVar(thisType);   //end of the zero-dimension, like int a = 10;
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
    }
}

void constVar(Type frontType) {
    if (thisSym == "CHARCON") {
        if (frontType != intCharType && frontType != charType)
            error("o", thisLine);
        CHARCON();
    } else {
        if (frontType != intCharType && frontType != intType)
            error("o", thisLine);
        INTEGER(); //can throw error
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<常量>\n");
}

void INTEGER() {    //SIGNED AND UNSIGNED INTEGER
    if (thisSym == "PLUS" || thisSym == "MINU") {
        plusAndMinus();
    }
    INTCON(); //can throw error
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<整数>\n");
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

void CHARCON() {
    char c = thisWord[0];
    bool valid = (c == '+' || c == '-' || c == '*' || c == '/' || c == '_' || isalnum(c));
    if (thisSym == "CHARCON" && valid) {
        getSym();
    } else if (thisSym == "CHARCON" && !valid) {
        getSym();
        error("a", missingDecideErrorLine());
    } else
        error("a", missingDecideErrorLine());
}

void STRCON() {
    if (thisSym != "STRCON") {
        error("a", missingDecideErrorLine());
    } else if (thisSym == "STRCON" && !validStr(thisWord)) {
        error("a", missingDecideErrorLine());
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
    IDENFR();
    checkConstAssign(peekWord(-1));
    if (thisSym == "ASSIGN") { //a = 1 + 2;
        ASSIGN();
        expression();
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
        error("in conditionStatement, not  started with IFTK");
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
        CHARCON();
        thisType = charType;
    } else if (thisSym == "INTCON" || ((thisSym == "PLUS" || thisSym == "MINU") && peekSym(1) == "INTCON")) {
        INTEGER();
        thisType = intType;
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
        RPARENT();
        thisType = intType; //只要参与运算，就是int型，例如(‘c’)的结果是整型
    } else if (thisSym == "IDENFR") {
        tablePt pt = IDENFR();
        if (pt == NULL) { //IDENFR not defined, error("c")emiteed in IDENFR();
            thisType = undefinedType;
        } else
            thisType = pt->type;
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
    if (thisSym == "PLUS" || thisSym == "MINU") {
        cntOp++;
        getSym();
    }
    Type thisItemType = item();
    while (thisSym == "PLUS" || thisSym == "MINU") {
        cntOp++;
        getSym(); item();
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<表达式>\n");

    if (cntOp == 0 && thisItemType == charType)
        return charType;
    else
        return intType;
}

Type item() {
    bool hasOp = false;
    Type factorType = factor();
    while (thisSym == "MULT" || thisSym == "DIV") {
        hasOp = true;
        getSym(); factor();
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
	if (!isUndefined) {
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
        } else { //printf '(' <字符串>
            STRCON();
            if (thisSym == "COMMA") { //printf '(' <字符串>,<表达式> ')'
                getSym(); expression();
            }
        }
        RPARENT();
    }
    if (grammarDealingDebug)
        fprintf(grammarDealingOUT, "<写语句>\n");
}

void scanfStatement() {
    if (thisSym == "SCANFTK") {
        getSym();
        LPARENT();
        IDENFR();
        checkConstAssign(peekWord(-1));
        RPARENT();
    } else
        error();
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

//grammar dealing functions end from here
//=====================================================
//word dealing functions start from here

int isAlpha(char c)
{
    return isalpha(c) || (c == '_');
}

int isBrackets(char c)
{
    if (bracketMap.count(c) > 0) {
        return bracketMap[c];
    }
    return 0;
}

int isRelation(char c)
{
    if (c == '=' || c == '!' || c == '<' || c == '>') {
        return 1;
    } else {
        return 0;
    }
}

char* processNum()
{
    int i = 0;
    memset(temp, 0, TEMPLEN);
    while(isdigit(in_line[col])) {
        temp[i] = in_line[col];
        i++;
        col++;
    }
    return temp;
}

char* processIdentifier()
{

    int i = 0;
    memset(temp, 0, TEMPLEN);
    while (isAlpha(in_line[col]) || isdigit(in_line[col])) {
        temp[i] = in_line[col];
        i++;
        col++;
    }
    return temp;
}

void storeObj(char *obj, int mode, string code, string word)
{
    if (obj == NULL) {
        ;//error
    } else if (mode == 1) {
        addToVec(code ,word);
    } else {
        string str = obj;
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        if (codeMap.count(str) > 0) {
            addToVec(codeMap[str], obj);
        } else {
            addToVec("IDENFR", obj);
        }
    }
    return ;
}

void addToVec(string code, string word)
{
    codeVec.push_back(code);
    wordVec.push_back(word);
    lineVec.push_back(line);
}

void printVec()
{
    for (int i = 0; i < wordVec.size(); i++) {
        fprintf(wordDealingOUT, "%s %s %d\n", codeVec[i].c_str(), wordVec[i].c_str(), lineVec[i]);
    }
}

char* processCharToken()
{
    int cnt = 0;
    col++;
    charToken[0] = in_line[col];
    charToken[1] = '\0';
    col++;
    if (in_line[col] != '\'') {
        ;//error
    } else {
        col++;
    }
    return charToken;
}

char* processString()
{
    col++;
    int i = 0;
    memset(temp, 0, TEMPLEN);
    while (in_line[col] != '\"') {
        temp[i] = in_line[col];
        i++;
        col++;
    }
    col++;
    return temp;
}

char* processReation()
{
    charToken[0] = in_line[col];
    col++;
    if (isRelation(in_line[col])) {
        charToken[1] = in_line[col];
        charToken[2] = '\0';
        col++;
        if (isRelation(in_line[col])) {
            //error
        }
    } else {
        charToken[1] = '\0';
    }
    return charToken;
}

void printSingleChar(char c)
{
    charToken[0] = c;
    charToken[1] = '\0';
    storeObj(charToken, 0, "", "");
    return ;
}
//word dealing functions end from here
