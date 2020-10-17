#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <algorithm>
#include <unordered_map>

using namespace std;

typedef struct retMessager {
    int ret;
    char *point;
    int line;
    int col;
}retMessage;

FILE *IN = fopen("testfile.txt", "r");
FILE *OUT = fopen("output.txt", "w");
#define MAXLEN 65536
#define TEMPLEN 65536
char in_line[MAXLEN + 5] = {0};
char temp[TEMPLEN + 5] = {0};
char charToken[3] = {'\0', '\0', '\0'};
int line = 0;
int col = 0;
std::unordered_map<char, int> bracketMap {
    {'\(', 1},
    {'\)', 2},
    {'\[', 3},
    {'\]', 4},
    {'\{', 5},
    {'\}', 6}
};
std::unordered_map<string, string> codeMap {
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

void consumeBlank();
int isAlpha(char c);
int isBrackets(char c);
int isRelation(char c);
char* processNum();
char* processIdentifier();
char* processCharToken();
char* processString();
char* processReation();
char* stringToLower(char *str);
void printObj(char *obj);
void printSingleChar(char c);

int main()
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
            //consumeBlank();
            if (isAlpha(in_line[col])) {
                char* identifier = processIdentifier();
                printObj(identifier);
            } else if (isdigit(in_line[col])) {
                char* num = processNum(); //col is added here
                printObj(num);
            } else if (in_line[col] == '\'') {
                char* curChar = processCharToken();
                fprintf(OUT, "CHARCON ");
                fputs(curChar, OUT);
                fputs("\n", OUT);
            } else if (in_line[col] == '\"') {
                processString();
                fprintf(OUT, "STRCON ");
                fputs(temp, OUT);
                fputs("\n", OUT);
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
                printObj(relation);
            } else {
                col++;
            }
        }
    }


    fclose(IN);
    fclose(OUT);
    return 0;
}

void consumeBlank()
{
    while (in_line[col] == ' ')
        col++;
    return ;
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

void printObj(char *obj)
{
    if (obj == NULL) {
        ;//error
    } else {
        if (isdigit(obj[0])) {
            fprintf(OUT, "INTCON %s\n", obj);
            return ;
        }
        string str = obj;
        transform(str.begin(), str.end(), str.begin(), ::tolower);
        if (codeMap.count(str) > 0) {
            fprintf(OUT, "%s %s\n", codeMap[str].c_str(), obj);
        } else {
            fprintf(OUT, "IDENFR %s\n", obj);
        }
    }
    return ;
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
    printObj(charToken);
    return ;
}
