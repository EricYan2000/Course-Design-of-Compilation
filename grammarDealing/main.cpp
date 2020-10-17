#include <iostream>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <set>

using namespace std;

typedef struct retMessager {
    int ret;
    char *point;
    int line;
    int col;
}retMessage;

FILE *IN = fopen("testfile.txt", "r");
FILE *wordDealingOUT = fopen("wordDealingOutput.txt", "w");
FILE *OUT = fopen("output.txt", "w");
#define MAXLEN 65536
#define TEMPLEN 8192
string thisSym = "";    //store the result of getSym(codeMap[symIndex]) in thisSym
string thisWord = "";   //store
char in_line[MAXLEN + 5] = {0};
char temp[TEMPLEN + 5] = {0};
char charToken[3] = {'\0', '\0', '\0'};
int line = 0;
int col = 0;
int vectorSize = 0; //size of wordMap and codeMap, represents the number of words read in
int symIndex = -1;  //used in nextSym to return codeMap[symIndex]. Added by 1 by getSym when every time used
vector<string> wordVec;
vector<string> codeVec;
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
set<string> retFuncNameSet;
set<string> noRetFuncNameSet;

bool readInDebug = false; //print the original code it has read
bool grammarDealingDebug = true;

int grammarDealing();
int getSym();  //get the next Sym from codeMap
string peekSym(int i);  //get number i element behind codeMap[symIndex]
string peekWord(int i);
void error();

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

void program();
void constDeclaration();
void varDeclaration();
void functions();
void mainFunction();
void constDefinition();
void IDENFR();
void INTCON();
void CHARCON();
void INTEGER();
void plusAndMinus();
void funcHead();
void retFunc();
void noRetFunc();
void funcTable();
void compoundStatement();
void varDefinition();
void constVar();
int isPresetVar();
void presetVarDefinition();
void noPresetVarDefinition();
void assignTwoDimensionWithConstVar();
void rowsOfStatements();
void typeSymbol();
void addRetFunc(string funcName);
bool isRetFuncRecorded(string funcName);
void addNoRetFunc(string funcName);
bool isNoRetFuncRecorded(string funcName);
void factor();
void retFuncStatement();
void expression();
void item();
void scanfStatement();
void printfStatement();
void returnStatement();
void stringCon();
void statement();
void assignStatement();
void conditionStatement();
void condition();
void relationOp();
void loopStatement();
void stepWidth();
void switchStatement();
void switchTable();
void switchDefault();
void subSwitch();
void noRetFuncStatement();
void inputFuncTable();
bool searchForAssign();

int main()
{
    wordDealing();

    grammarDealing();   //getSym() in its first line

    return 0;
}

void error() {
    printf("maybe there's something wrong\n");
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
    return 0;
}

//grammar dealing functions start from here

int getSym() //thisSym判断才是真正的读，因此getSym的实质还是预读。因此，在getSym下一个时，输出上一个getSym
{
    symIndex++;
    if (symIndex <= vectorSize) {
        thisSym = codeVec[symIndex];
        thisWord = wordVec[symIndex];
        if (grammarDealingDebug && symIndex >= 1)
            fprintf(OUT, "%s %s\n", codeVec[symIndex - 1].c_str(), wordVec[symIndex - 1].c_str());
        return 1;
    } else {
        thisSym = "";
        return 0;
    }
}

string peekSym(int i)
{
    if (symIndex + i < vectorSize) {
        return codeVec[symIndex + i];
    } else {
        return "";
    }
}

string peekWord(int i)
{
    if (symIndex + i < vectorSize) {
        return wordVec[symIndex + i];
    } else {
        return "";
    }
}

void addRetFunc(string funcName) {
    retFuncNameSet.insert(funcName);
}

bool isRetFuncRecorded(string funcName) {
    return retFuncNameSet.count(funcName) != 0;
}

void addNoRetFunc(string funcName) {
    noRetFuncNameSet.insert(funcName);
}

bool isNoRetFuncRecorded(string funcName) {
    return noRetFuncNameSet.count(funcName);
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
        fprintf(OUT, "<程序>\n");
}

void constDeclaration() {
    if (thisSym == "CONSTTK") {
        getSym();
        constDefinition();
        if (thisSym != "SEMICN")
            error();
        else
            getSym();
    }
    while (thisSym == "CONSTTK") { //extra consts
        getSym();
        constDefinition();
        if (thisSym != "SEMICN")
            error();
        else
            getSym();
    }
    if (grammarDealingDebug)
        fprintf(OUT, "<常量说明>\n");
}

void constDefinition() {
    if (thisSym == "INTTK") { //int a = 10, b = 15;
        getSym(); IDENFR();
        if (thisSym == "ASSIGN") {
            getSym(); INTEGER();
            while (thisSym == "COMMA") {
                getSym(); IDENFR();
                if (thisSym == "ASSIGN") {
                    getSym(); INTEGER();
                } else
                    error();
            }
        } else
            error();
    } else if (thisSym == "CHARTK") { //char c = 'a', p = 'z'
        getSym(); IDENFR();
        if (thisSym == "ASSIGN") {
            getSym(); CHARCON();
            while (thisSym == "COMMA") {
                getSym(); IDENFR();
                if (thisSym == "ASSIGN") {
                    getSym(); CHARCON();
                } else
                    error();
            }
        } else
            error();
    } else
        error();
    if (grammarDealingDebug && thisSym != "COMMA")
        fprintf(OUT, "<常量定义>\n");
}

void varDeclaration() {
    if ((thisSym == "INTTK" || thisSym == "CHARTK")
        && peekSym(1) == "IDENFR" && peekSym(2) != "LPARENT") { //make sure it's not a function
        varDefinition();
        if (thisSym == "SEMICN") { // ';' at the end of each definition
            getSym();
            while ((thisSym == "INTTK" || thisSym == "CHARTK")
                   && peekSym(1) == "IDENFR" && peekSym(2) != "LPARENT") {
                varDefinition();
                if (thisSym == "SEMICN")
                    getSym();
                else
                    error();
            }
        } else
            error();
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<变量说明>\n");
}

void varDefinition() {
    if (isPresetVar())
        presetVarDefinition();
    else
        noPresetVarDefinition();
    if (grammarDealingDebug)
        fprintf(OUT, "<变量定义>\n");
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
    if (thisSym == "INTTK" || thisSym == "CHARTK") {
        getSym(); IDENFR();
        if (thisSym == "ASSIGN") {
            getSym(); constVar();   //end of the zero-dimension, like int a = 10;
        } else if (thisSym == "LBRACK") { //int a[
            getSym(); INTCON();
            if (thisSym == "RBRACK") { //int a[5]
                getSym();
                if (thisSym == "ASSIGN") { //int a[5] =
                    getSym();
                    if (thisSym == "LBRACE") { //int a[5] = {
                        getSym(); constVar();
                        while (thisSym == "COMMA") {
                            getSym(); constVar();
                        }
                        if (thisSym == "RBRACE") //int a[5] = {1,2}
                            getSym();
                        else
                            error();
                    } else
                        error();
                } else if (thisSym == "LBRACK") { //int a[5][
                    getSym(); INTCON();
                    if (thisSym == "RBRACK") { //int a[5][7]
                        getSym();
                        if (thisSym == "ASSIGN") { //int a[5][7] =
                            getSym();
                            assignTwoDimensionWithConstVar(); //ends with getSym()
                        } else
                            error();
                    } else
                        error();
                } else
                    error();
            } else
                error();
        } else
            error();
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<变量定义及初始化>\n");
}

void assignTwoDimensionWithConstVar() { //int a[5][7] =  //used in presetVarDefinition
    if (thisSym == "LBRACE") {
        getSym();
        if (thisSym == "LBRACE") { //int a[5][7] = {{
            getSym(); constVar();
            while (thisSym == "COMMA") {
                getSym(); constVar();
            }
            if (thisSym == "RBRACE")
                getSym();
            else
                error();
            while (thisSym == "COMMA") { //int a[5][7] = {{1,2} ,
                getSym();
                if (thisSym == "LBRACE") { //int a[5][7] = {{1,2} , {
                    getSym(); constVar();
                    while (thisSym == "COMMA") { //int a[5][7] = {{1,2} , {22,3,
                        getSym(); constVar();
                    }
                    if (thisSym == "RBRACE") //int a[5][7] = {{1,2} , {22,3,4}
                        getSym();
                    else
                        error();
                } else
                    error();
            }
            if (thisSym == "RBRACE") //should be int a[5][7] = {{1,2} , {22,3,4}}
                getSym();
            else
                error();
        } else
            error();
    } else
        error();
}

void noPresetVarDefinition() {
    if (thisSym == "INTTK" || thisSym == "CHARTK") {
        getSym();IDENFR(); //zero-dimension, like c
        if (thisSym == "LBRACK") {
            getSym();INTCON();
            if (thisSym == "RBRACK") { //end of the one-dimension, like c[10]
                getSym();
                if (thisSym == "LBRACK") {
                    getSym();INTCON();
                    if (thisSym == "RBRACK") {
                        getSym(); //end of the two-dimension, like c[10][10]
                    } else
                        error();
                }
            } else
                error();
        }
        while (thisSym == "COMMA") { //extra-length
            getSym(); IDENFR(); //zero-dimension, like c
            if (thisSym == "LBRACK") {
                getSym(); INTCON();
                if (thisSym == "RBRACK") { //end of the one-dimension, like c[10]
                    getSym();
                    if (thisSym == "LBRACK") {
                        getSym(); INTCON();
                        if (thisSym == "RBRACK") {
                            getSym(); //end of the two-dimension, like c[10][10]
                        } else
                            error();
                    }
                } else
                    error();
            }
        }
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<变量定义无初始化>\n");
}

void constVar() {
    if (thisSym == "CHARCON") {
        getSym();
    } else {
        INTEGER();
    }
    if (grammarDealingDebug)
        fprintf(OUT, "<常量>\n");
}

void INTEGER() {    //SIGNED AND UNSIGNED INTEGER
    if (thisSym == "PLUS" || thisSym == "MINU") {
        plusAndMinus();
    }
    INTCON();
    if (grammarDealingDebug)
        fprintf(OUT, "<整数>\n");
}

void INTCON() {  //UNSIGNED INTEGER
    if (thisSym == "INTCON") {
        getSym();
    }
    else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<无符号整数>\n");
}

void CHARCON() {
    if (thisSym == "CHARCON") {
        getSym();
    } else
        error();
}

void IDENFR() {
    if (thisSym == "IDENFR") {
        getSym();
    } else
        error();
}

void funcHead() {
    if (thisSym == "INTTK" || thisSym == "CHARTK") {
        getSym();IDENFR();
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<声明头部>\n");
}

void functions() {  //self-created function
    if (thisSym == "VOIDTK") {
        noRetFunc();
    } else if (thisSym == "INTTK" || thisSym == "CHARTK") {
        retFunc();
    } else
        error();
}

void noRetFunc() {
    //嵌套问题
    string funcName = peekWord(1);
    int cnt = noRetFuncNameSet.count(funcName);  // != 0 means, there's already a legal function calls funcName
    addNoRetFunc(funcName);
    int errorFlag = 0;
    if (thisSym == "VOIDTK") {
        getSym(); IDENFR();
        if (thisSym == "LPARENT") {
            getSym(); funcTable();
            if (thisSym == "RPARENT") {
                getSym();
                if (thisSym == "LBRACE") {
                    getSym(); compoundStatement();
                    if (thisSym == "RBRACE") {
                        //addNoRetFunc(funcName); //moved to the front
                        getSym();
                    } else {
                        error(); errorFlag = 1;
                    }
                } else {
                    error(); errorFlag = 1;
                }
            } else {
                error(); errorFlag = 1;
            }
        } else {
            error(); errorFlag = 1;
        }
    } else {
        error(); errorFlag = 1;
    }
    /*
    if (errorFlag == 1 && cnt == 0)
        noRetFuncNameSet.erase(funcName); //this function error + no previous legal function
    */
    if (grammarDealingDebug)
        fprintf(OUT, "<无返回值函数定义>\n");
    printf("noRetFunc didn't erase illegal funcName\n");
}

void retFunc() {
    //解决fib嵌套定义的问题
    string funcName = peekWord(1);
    int cnt = retFuncNameSet.count(funcName);  // != 0 means, there's already a legal function calls funcName
    addRetFunc(funcName);
    int errorFlag = 0;
    funcHead();
    if (thisSym == "LPARENT") {
        getSym(); funcTable();
        if (thisSym == "RPARENT") {
            getSym();
            if (thisSym == "LBRACE") {
                getSym(); compoundStatement();
                if (thisSym == "RBRACE") {
                    //addRetFunc(funcName); //moved to the front
                    getSym();
                }
                else {
                    error(); errorFlag = 1;
                }
            } else {
                error(); errorFlag = 1;
            }
        } else {
            error(); errorFlag = 1;
        }
    } else {
        error(); errorFlag = 1;
    }
    /*
    if (errorFlag == 1 && cnt == 0)
        retFuncNameSet.erase(funcName); //this function error + no previous legal function
        */
    if (grammarDealingDebug)
        fprintf(OUT, "<有返回值函数定义>\n");
    printf("retFunc didn't erase illegal funcName\n");
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
        fprintf(OUT, "<复合语句>\n");
}

void rowsOfStatements() {
    while (thisSym == "FORTK" || thisSym == "WHILETK" || thisSym == "IFTK" || thisSym == "SWITCHTK"
            || thisSym == "SEMICN" || thisSym == "RETURNTK" || thisSym == "LBRACE"
            || thisSym == "SCANFTK" || thisSym == "PRINTFTK"
            || (thisSym == "IDENFR" && isRetFuncRecorded(thisWord) && peekSym(1) == "LPARENT")
            || (thisSym == "IDENFR" && isNoRetFuncRecorded(thisWord) && peekSym(1) == "LPARENT")
            || (thisSym == "IDENFR" && searchForAssign()))
        statement();
    if (grammarDealingDebug)
        fprintf(OUT, "<语句列>\n");
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
        if (thisSym == "SEMICN")
            getSym();
        else
            error();
    } else if (thisSym == "IDENFR" && isNoRetFuncRecorded(thisWord) && peekSym(1) == "LPARENT") {
        noRetFuncStatement();
        if (thisSym == "SEMICN")
            getSym();
        else
            error();
    } else if (thisSym == "SEMICN") { //<空>;
        getSym();
    } else if (thisSym == "RETURNTK") {
        returnStatement();
        if (thisSym == "SEMICN") {
            getSym();
        } else
            error();
    } else if (thisSym == "SCANFTK") {
        scanfStatement();
        if (thisSym == "SEMICN") {
            getSym();
        } else
            error();
    } else if (thisSym == "PRINTFTK") {
        printfStatement();
        if (thisSym == "SEMICN") {
            getSym();
        } else
            error();
    } else if (thisSym == "SWITCHTK") {
        switchStatement();
    } else if (thisSym == "LBRACE") {
        getSym(); rowsOfStatements();
        if (thisSym == "RBRACE") {
            getSym();
        } else
            error();
    } else {
        assignStatement();
        if (thisSym == "SEMICN") {
            getSym();
        } else
            error();
    }
    if (grammarDealingDebug)
        fprintf(OUT, "<语句>\n");
}

void assignStatement() {
    IDENFR();
    if (thisSym == "ASSIGN") { //a = 1 + 2;
        getSym(); expression();
    } else if (thisSym == "LBRACK") {
        getSym(); expression();
        if (thisSym == "RBRACK") {
            getSym();
            if (thisSym == "ASSIGN") { //a[5] =
                getSym(); expression();
            } else if (thisSym == "LBRACK") { //a[5][
                getSym(); expression();
                if (thisSym == "RBRACK") { //a[5][6]
                    getSym();
                    if (thisSym == "ASSIGN") {
                        getSym(); expression();
                    } else
                        error();
                } else
                    error();
            } else
                error();
        } else
            error();
    } else
        error();

    if (grammarDealingDebug)
        fprintf(OUT, "<赋值语句>\n");
}

void conditionStatement() {
    if (thisSym == "IFTK") {
        getSym();
        if (thisSym == "LPARENT") {
            getSym(); condition();
            if (thisSym == "RPARENT") {
                getSym(); statement();
                if (thisSym == "ELSETK") {
                    getSym(); statement();
                }
            } else
                error();
        } else
            error();
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<条件语句>\n");
}

void condition() {
    expression();
    relationOp();
    expression();
    if (grammarDealingDebug)
        fprintf(OUT, "<条件>\n");
}

void loopStatement() {
    if (thisSym == "WHILETK") {
        getSym();
        if (thisSym == "LPARENT") {
            getSym(); condition();
            if (thisSym == "RPARENT") {
                getSym(); statement();
            } else
                error();
        } else
            error();
    } else if (thisSym == "FORTK") {
        getSym();
        if (thisSym == "LPARENT") {
            getSym(); IDENFR();
            if (thisSym == "ASSIGN") {
                getSym(); expression();
                if (thisSym == "SEMICN") {
                    getSym(); condition();
                    if (thisSym == "SEMICN") {
                        getSym(); IDENFR();
                        if (thisSym == "ASSIGN") {
                            getSym(); IDENFR();
                            if (thisSym == "PLUS" || thisSym == "MINU") {
                                getSym(); stepWidth();
                                if (thisSym == "RPARENT") {
                                    getSym(); statement();
                                } else
                                    error();
                            } else
                                error();
                        } else
                            error();
                    } else
                        error();
                } else
                    error();
            } else
                error();
        } else
            error();
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<循环语句>\n");
}

void stepWidth() {
    INTCON();
    if (grammarDealingDebug)
        fprintf(OUT, "<步长>\n");
}

void switchStatement() {
    if (thisSym == "SWITCHTK") {
        getSym();
        if (thisSym == "LPARENT") {
            getSym(); expression();
            if (thisSym == "RPARENT") {
                getSym();
                if (thisSym == "LBRACE") {
                    getSym(); switchTable(); switchDefault();
                    if (thisSym == "RBRACE") {
                        getSym();
                    } else
                        error();
                } else
                    error();
            } else
                error();
        } else
            error();
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<情况语句>\n");
}

void switchTable() {
    subSwitch();
    while (thisSym == "CASETK") {
        subSwitch();
    }
    if (grammarDealingDebug)
        fprintf(OUT, "<情况表>\n");
}

void subSwitch() {
    if (thisSym == "CASETK") {
        getSym(); constVar();
        if (thisSym == "COLON") {
            getSym(); statement();
        } else
            error();
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<情况子语句>\n");
}

void switchDefault() {
    if (thisSym == "DEFAULTTK") {
        getSym();
        if (thisSym == "COLON") {
            getSym(); statement();
        } else
            error();
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<缺省>\n");
}

void relationOp() {
    if (thisSym == "LSS" || thisSym == "LEQ" || thisSym == "GRE"
        || thisSym == "GEQ" || thisSym == "EQL" || thisSym == "NEQ" )
        getSym();
    else
        error();
}

void factor() {
    if (thisSym == "CHARCON") {
        getSym();
    } else if (thisSym == "INTCON" || ((thisSym == "PLUS" || thisSym == "MINU") && peekSym(1) == "INTCON")) {
        INTEGER();
    } else if (thisSym == "IDENFR" && peekSym(1) == "LPARENT" && isRetFuncRecorded(thisWord)) {
        retFuncStatement();
    } else if (thisSym == "LPARENT") {
        getSym(); expression();
        if (thisSym == "RPARENT") {
            getSym();
        } else
            error();
    } else if (thisSym == "IDENFR") {
            IDENFR(); // a. already getSym() in IDENFR().
            if (thisSym == "LBRACK") {
                getSym(); expression();
                if (thisSym == "RBRACK") { //a[(3+4)]
                    getSym();
                    if (thisSym == "LBRACK") {
                        getSym(); expression();
                        if (thisSym == "RBRACK") { //a[(3+4)][5]
                            getSym();
                        } else
                            error();
                    }
                } else
                    error();
            }
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<因子>\n");
}

void expression() {
    if (thisSym == "PLUS" || thisSym == "MINU")
        getSym();
    item();
    while (thisSym == "PLUS" || thisSym == "MINU") {
        getSym(); item();
    }
    if (grammarDealingDebug)
        fprintf(OUT, "<表达式>\n");
}

void item() {
    factor();
    while (thisSym == "MULT" || thisSym == "DIV") {
        getSym(); factor();
    }
    if (grammarDealingDebug)
        fprintf(OUT, "<项>\n");
}

void retFuncStatement() {
    IDENFR();
    if (thisSym == "LPARENT") {
        getSym();
        inputFuncTable();
        if (thisSym == "RPARENT") {
            getSym();
        } else
            error();
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<有返回值函数调用语句>\n");
}

void noRetFuncStatement() {
    IDENFR();
    if (thisSym == "LPARENT") {
        getSym();
        inputFuncTable();
        if (thisSym == "RPARENT") {
            getSym();
        } else
            error();
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<无返回值函数调用语句>\n");
}

void inputFuncTable() {
    if (thisSym == "RPARENT") //为空
        ; //do not getSym()
    else
        expression();
    while (thisSym == "COMMA") {
        getSym(); expression();
    }
    if (grammarDealingDebug)
        fprintf(OUT, "<值参数表>\n");
}


void printfStatement() {
    if (thisSym == "PRINTFTK") {
        getSym();
        if (thisSym == "LPARENT") {
            getSym();
            if (thisSym != "STRCON") { //printf '(' <表达式>
                expression(); //thisSym = ')'
            } else { //printf '(' <字符串>
                stringCon();
                if (thisSym == "COMMA") { //printf '(' <字符串>,<表达式> ')'
                    getSym(); expression();
                }
            }
            if (thisSym == "RPARENT") //check ')'
                getSym();
            else
                error();
        } else
            error();
    }
    if (grammarDealingDebug)
        fprintf(OUT, "<写语句>\n");
}

void scanfStatement() {
    if (thisSym == "SCANFTK") {
        getSym();
        if (thisSym == "LPARENT") {
            getSym(); IDENFR();
            if (thisSym == "RPARENT")
                getSym();
            else
                error();
        } else
            error();
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<读语句>\n");
}

void stringCon() {
    if (thisSym == "STRCON")
        getSym();
    else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<字符串>\n");
}

void returnStatement() {
    if (thisSym == "RETURNTK") {
        getSym();
        if (thisSym == "LPARENT") {
            getSym(); expression();
            if (thisSym == "RPARENT")
                getSym();
            else
                error();
        }
    } else
        error();
    if (grammarDealingDebug)
        fprintf(OUT, "<返回语句>\n");
}

void funcTable() {
    if (thisSym == "INTTK" || thisSym == "CHARTK") {
        getSym(); IDENFR();
        while (thisSym == "COMMA") {
            getSym(); typeSymbol(); IDENFR(); //IDENFN will get the nextSym
        }
    } //else do nothing. Because if funcTable is empty, thisSym should already be ')'
    if (grammarDealingDebug)
        fprintf(OUT, "<参数表>\n");
}

void typeSymbol() {
    if (thisSym == "INTTK" || thisSym == "CHARTK") {
        getSym();
    } else
        error();
}

void mainFunction() {
    if (thisSym == "VOIDTK") getSym();
    else error();
    if (thisSym == "MAINTK") getSym();
    else error();
    if (thisSym == "LPARENT") getSym();
    else error();
    if (thisSym == "RPARENT") getSym();
    else error();
    if (thisSym == "LBRACE") {
        getSym(); compoundStatement();
        if (thisSym == "RBRACE") {
            getSym();
        } else
            error();
    }
    else error();
    if (grammarDealingDebug)
        fprintf(OUT, "<主函数>\n");
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
}

void printVec()
{
    for (int i = 0; i < wordVec.size(); i++) {
        fprintf(wordDealingOUT, "%s %s\n", codeVec[i].c_str(), wordVec[i].c_str());
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