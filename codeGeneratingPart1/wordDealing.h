//
// Created by SAMSUNG on 2020/11/9.
//

#ifndef SPLITERRORDEALING_WORDDEALING_H
#define SPLITERRORDEALING_WORDDEALING_H

#include <unordered_map>
#include <string.h>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include "config.h"

using namespace std;

#define MAXLEN 65536
#define TEMPLEN 8192
extern string thisSym;      //store the result of getSym(codeMap[symIndex]) in thisSym
extern string thisWord;     //store the text of the word
extern int thisLine;        //store the line number of the current word
extern int vectorSize;      //size of wordMap and codeMap, represents the number of words read in
extern vector<string> wordVec;  //the result of wordDealing (word)
extern vector<string> codeVec;  //the result of wordDealing (token like WHILETK)
extern vector<int> lineVec;


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

#endif //SPLITERRORDEALING_WORDDEALING_H
