//
// Created by SAMSUNG on 2020/12/8.
//

#include "myVectors.h"

vector<string> wordVec;
vector<string> codeVec;
vector<int> lineVec;

int symIndex = -1;
int vectorSize = 0;

string thisSym = "";    //store the result of getSym(codeMap[symIndex]) in thisSym
string thisWord = "";   //store the text of the word
int thisLine = 0;       //store the line number of the current word

int getSym() //thisSym判断才是真正的读，因此getSym的实质还是预读。因此，在getSym下一个时，输出上一个getSym
{
    symIndex++;
    if (symIndex <= vectorSize) {
        if (symIndex < vectorSize) {
            thisSym = codeVec[symIndex];
            thisWord = wordVec[symIndex];
            if (thisSym == "IDENFR")
                transform(thisWord.begin(), thisWord.end(), thisWord.begin(), ::tolower);
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
    if (symIndex + i < vectorSize) {
        string str = wordVec[symIndex + i];
        if (codeVec[symIndex + i] == "IDENFR")
            transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
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

void pushVec(string code, string word, int line_num)
{
    codeVec.push_back(code);
    wordVec.push_back(word);
    lineVec.push_back(line_num);
}

void debugVec()
{
    for (int i = 0; i < wordVec.size(); i++) {
        fprintf(wordDealingDebugFile, "%s %s %d\n", codeVec[i].c_str(), wordVec[i].c_str(), lineVec[i]);
    }
    fclose(wordDealingDebugFile);
}

void calculateVecSize() {
    vectorSize = wordVec.size();
}

