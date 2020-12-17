//
// Created by SAMSUNG on 2020/11/9.
//

#ifndef SPLITERRORDEALING_CONFIG_H
#define SPLITERRORDEALING_CONFIG_H

#include <stdio.h>

using namespace std;

extern bool wordDealingDebug;      //print wordVec & codeVec
extern bool readInDebug; //print the original code it has read
extern bool grammarDealingDebug;
extern bool tableDebug;
extern bool braceDebug;
extern bool paraTableDebug;
extern bool itemStackDebug;
extern bool bridgeCodeDebug;
extern bool mappingDebug;

extern const char *inputFileName;
extern const char *wordDealingOutput;
extern const char *grammarDealingOutput;
extern const char *errorDealingOutput;
extern const char *mipsOutput;

extern FILE *grammarDealingOUT;
extern FILE *errorOUT;
extern FILE *mipsOUT;
extern FILE *IN;
extern FILE *wordDealingOUT;
extern FILE *wordDealingDebugFile;

#endif //SPLITERRORDEALING_CONFIG_H
