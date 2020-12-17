//
// Created by SAMSUNG on 2020/11/9.
//

#include "wordDealing.h"

using namespace std;

char in_line[MAXLEN + 5] = {0};
char temp[TEMPLEN + 5] = {0};
char charToken[3] = {'\0', '\0', '\0'};
int line = 0;
int col = 0;

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

    //all the words and codes are loaded into vector in the function addToVec()
    if (wordDealingDebug) {
        debugVec();     //debug
    }
    calculateVecSize();
    fclose(IN);
    fclose(wordDealingOUT);
    return 0;
}

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
        addToVec(code ,word, line);
    } else {
        string str = obj;
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        if (codeMap.count(str) > 0) {
            addToVec(codeMap[str], obj, line);
        } else {
            addToVec("IDENFR", obj, line);
        }
    }
    return ;
}

void addToVec(string code, string word, int line) {
    pushVec(code, word, line);
    printWord(code, word);
}

void printWord(string code, string word) {
    fprintf(wordDealingOUT, "%s %s\n", code.c_str(), word.c_str());
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