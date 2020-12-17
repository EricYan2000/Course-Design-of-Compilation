//
// Created by SAMSUNG on 2020/12/8.
//

#ifndef CODEGENERATING_MYVECTORS_H
#define CODEGENERATING_MYVECTORS_H

#include <unordered_map>
#include <string.h>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include "config.h"

using namespace std;

extern vector<string> wordVec;  //the result of wordDealing (word)
extern vector<string> codeVec;  //the result of wordDealing (token like WHILETK)
extern vector<int> lineVec;

extern int symIndex;
extern int vectorSize;      //size of wordMap and codeMap, represents the number of words read in

extern string thisSym;      //store the result of getSym(codeMap[symIndex]) in thisSym
extern string thisWord;     //store the text of the word
extern int thisLine;        //store the line number of the current word

int getSym();  //get the next Sym from codeMap
string peekSym(int i);  //get number i element behind codeMap[symIndex]
string peekWord(int i);
int peekLine(int i);

void pushVec(string code, string word, int line_num);    //store words and codes in to wordVec and codeVec
void debugVec();    //print words and codes to wordDealingOutput.txt

void calculateVecSize();

#endif //CODEGENERATING_MYVECTORS_H
