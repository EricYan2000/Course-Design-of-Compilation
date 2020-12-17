//
// Created by SAMSUNG on 2020/11/9.
//

#include "config.h"

using namespace std;

bool wordDealingDebug = false;  //print [code, word, line] from codeVec, wordVec, lineVec
//in codeGenerating version, all words are parsed into lower case before there were stored into vectors mentioned above
bool readInDebug = false;       //print the original code it has read
bool grammarDealingDebug = true;
bool tableDebug = false;
bool braceDebug = false;
bool paraTableDebug = false;
bool itemStackDebug = false;
bool bridgeCodeDebug = true;
bool mappingDebug = true;

const char *inputFileName = "testfile.txt";
const char *wordDealingOutput = "wordDealing.txt";
const char *grammarDealingOutput = "grammarDealing.txt";
const char *errorDealingOutput = "error.txt";
const char *mipsOutput = "mips.txt";

FILE *grammarDealingOUT = fopen(grammarDealingOutput, "w");
FILE *errorOUT = fopen(errorDealingOutput, "w");
FILE *mipsOUT = fopen(mipsOutput, "w");
FILE *IN = fopen(inputFileName, "r");
FILE *wordDealingOUT = fopen(wordDealingOutput, "w");
FILE *wordDealingDebugFile = fopen("wordDealingDebug.txt", "w");

