//
// Created by SAMSUNG on 2020/11/10.
//

#include "quadruple.h"
#include "config.h"
#include "memMap.h"
#include <stdlib.h>

vector<int> spaceStack;

//注意此处做了让步,a[5]的row_num = 5, col_num = 0

//=======quaItem starts from here=======
quaItem::quaItem(string name, Type type, int unit_bytes, int row_num, int col_num) {
    this->name = name;
    this->type = type;
    this->unit_bytes = unit_bytes;
    this->row_num = row_num;
    this->col_num = col_num;
}

string quaItem::getName() {
    return this->name;
}

Type quaItem::getType() {
    return this->type;
}

int quaItem::getRowNum() {
    return this->row_num;
}

int quaItem::getColNum() {
    return this->col_num;
}

int quaItem::getUnitBytes() {
    return this->unit_bytes;
}

int quaItem::getSize() {
    int size = 0;
    if (row_num == -1 && col_num == -1) //a
        size = this->unit_bytes;
    else if (row_num != -1 && col_num == -1) //a[5]
        size = this->unit_bytes * this->row_num;    //见"让步"
    else //a[3][4]
        size = this->unit_bytes * this->col_num * this->row_num;
    printf("%s , size is %d\n", getName().c_str(), size);
    return size;
}

void quaItem::setType(Type type) {
    this->type = type;
}

quaItem *placeHolderQuaItem = new quaItem("placeholder", undefinedType, 0);

//=====quadruple starts from here=====
quadruple::quadruple(string op, quaItem *item1, quaItem *item2, quaItem *item3, string arrayName, Type arrayType) {
    this->op = op;
    this->item1 = item1;
    this->item2 = item2;
    this->item3 = item3;
    this->arrayName = arrayName; //for assignArray
    this->arrayType = arrayType;
    Type item3Type = undefinedType;
    if (op == "+" || op == "-" || op == "*" || op == "/") {
        this->kind = calculateKind;
        item3Type = intType;
        quadrupleCnt++;
        this->item3 = new quaItem("T" + to_string(quadrupleCnt), item3Type);
    } else if (op == "printf") {
        this->kind = printfKind;
        item3Type = printfType;
        this->item3 = new quaItem("printf", item3Type);
    } else if (op == "scanf") {
        this->kind = scanfKind;
        item3Type = scanfType;
        this->item3 = new quaItem("scanf", item3Type);
    } else if (op == "defineConstInt") {
        this->kind = constIntDefinition;
        this->item3 = item1;
    }  else if (op == "defineConstChar") {
        this->kind = constCharDefinition;
        this->item3 = item1;
    } else if (op == "defineVarInt") {
        this->kind = varIntDefinition;
        this->item3 = item1;
    } else if (op == "defineVarChar") {
        this->kind = varCharDefinition;
        this->item3 = item1;
    } else if (op == "assign") {
        this->kind = assignKind;
        this->item3 = item1;
    } else if (op == "bge" || op == "bgt" || op == "ble" || op == "blt" ||
                op == "bne" || op == "beq" ) {
        this->kind = branchKind; //3 items already set
    } else if (op == "set_label") {
        this->kind = labelKind;
        this->item3 = item1;
    } else if (op == "jump") {
        this->kind = jumpKind;
        this->item3 = this->item1;
    } else if (op == "defineIntArray") {
        this->kind = intArrayDefinition;
        this->item3 = item1;
    } else if (op == "defineCharArray") {
        this->kind = charArrayDefinition;
        this->item3 = item1;
    } else if (op == "assignArray") {
        this->kind = assignArrayKind;
    } else if (op == "useArray") {
        this->kind = useArrayKind;
        quadrupleCnt++;
        this->item3 = new quaItem("T" + to_string(quadrupleCnt), item3Type);
        setItem(&searchItem, arrayName, undefinedType, 4, false, false, 0, 0);
        tablePt pt = find(searchItem);
        Type originalType = pt->type;
        if (originalType == intArrayType)
            this->item3->setType(intType);
        else
            this->item3->setType(charType);
    } else if (op == "getReturnValue") {
        this->kind = getReturnValueKind;
        quadrupleCnt++;
        this->item3 = new quaItem("T" + to_string(quadrupleCnt), arrayType);
    } else if (op == "mainFunc") {
        this->kind = mainEntranceKind;
        this->item3 = new quaItem("main_Entrance", labelType, 0);
    } else if (op == "retFuncCall" || op == "noRetFuncCall") {
        this->kind = funcCallKind;
        this->item3 = item1;
    } else if (op == "funcDef") {
        this->kind = funcDefKind;
        this->item3 = item1;
    } else if (op == "paraInFuncDef") {
        this->kind = paraDefKind;
        this->item3 = item1;
    } else if (op == "paraInFuncCall") {
        this->kind = paraInputKind;
        this->item3 = item1;
    } else if (op == "mainReturn") {
        this->kind = mainReturnKind;
        this->item3 = new quaItem("mainReturn", undefinedType, 0);
    } else if (op == "noRetFuncReturn") {
        this->kind = noRetFuncReturn;
        this->item3 = new quaItem("noRetFuncReturn", undefinedType, 0);
    } else if (op == "retFuncReturn") {
        this->kind = retFuncReturn;
        this->item3 = item1;
        //this->item3->setType(this->item2->getType());
    } else if (op == "funcEnd") {
        this->kind = funcEndKind;
        this->item3 = new quaItem("funcEnd", undefinedType, 0);
    } else if (op == "jal") {
        this->kind = jalKind;
        this->item3 = this->item1;
    } else if (op == "funcCallHeadMark") {
        this->kind = funcCallHeadMarkKind;
        this->item3 = this->item1;
    } else {
        printf("quadruple type undecided\n");
    }
}

string quadruple::getName() {
    if (this->kind == assignArrayKind)
        return this->arrayName;
    return this->item3->getName();
}

Type quadruple::getType() {
    return this->item3->getType();
}

quadrupleKind quadruple::getKind() {
    return this->kind;
}

quaItem* quadruple::toQuaItem() {
    return this->item3;
}

quaItem* quadruple::getItem1() {
    return this->item1;
}

quaItem* quadruple::getItem2() {
    return this->item2;
}

quaItem* quadruple::getItem3() {
    return this->item3;
}

string quadruple::toString() {
    if (this->kind == calculateKind) {
        if (this->item1 != nullptr && this->item2 != nullptr && this->item3 != nullptr)
            return this->item3->getName() + " = " + this->item1->getName() + " " + op
                   + " " + this->item2->getName() + "\n";
        else
            return "calculateKind toString not implemented\n";
    } else if (this->kind == printfKind) {
        string ret = this->op + " " + this->item1->getName(); //printf("hello")
        if (this->item2 == nullptr) {
            ret = ret + "\n";
        } else {
            ret = ret + " " + this->item2->getName() + "\n"; //print("hello", a)
        }
        return ret;
    } else if (this->kind == scanfKind) {
        return this->op + " " + this->item1->getName() + "\n";
    } else if (this->kind == constIntDefinition || this->kind == constCharDefinition) {
        string token = (this->item2->getType() == constInt) ? "int" : "char";
        return "const " + token + " " + this->item1->getName()
                    + " = " + this->item2->getName() + "\n";
    } else if (this->kind == varIntDefinition || this->kind == varCharDefinition) {
        string token = (this->item1->getType() == intType) ? "int" : "char";
        string ret =  "var " + token + " " + this->item1->getName();
        if (this->item2 != nullptr) {
            ret = ret  + " = " + this->item2->getName();
        }
        return ret + "\n";
    } else if (this->kind == assignKind) {
        if (this->item3->getType() == intArrayType ||this->item3->getType() == charArrayType) {
            int row_num = this->item3->getRowNum();
            int col_num = this->item3->getColNum();
            if (col_num == -1)
                return this->item3->getName() + "[" + to_string(row_num) + "]" + " = " + this->item2->getName() + "\n";
            else
                return this->item3->getName() + "[" + to_string(row_num) + "][" + to_string(col_num) + "]"
                    + " = " + this->item2->getName() + "\n";
        } else
            return this->item3->getName() + " = " + this->item2->getName() + "\n";
    } else if (this->kind == branchKind) {
        return this->op + " " + item1->getName() + ", " + item2->getName() + ", "
                + item3->getName() + "\n";
    }  else if (this->kind == jumpKind) {
        return "j " + this->item3->getName() + "\n";
    } else if (this->kind == labelKind) {
        return this->item3->getName() + ":\n";
    } else if (this->kind == intArrayDefinition || this->kind == charArrayDefinition) {
        string token = (this->item1->getType() == intArrayType) ? "int" : "char";
        int row_num = this->item1->getRowNum();
        int col_num = this->item1->getColNum();
        string name = this->item1->getName();
        if (row_num != -1)
            name = name + "[" + to_string(row_num) + "]";
        if (col_num != -1)
            name = name + "[" + to_string(col_num) + "]";
        return "var " + token + " " + name + "\n";
    } else if (this->kind == assignArrayKind) {
        if (this->item2 == placeHolderQuaItem) { //a[1] = t
            return arrayName + "[" + item1->getName() + "]" + " = " + item3->getName() + "\n";
        } else {
            return arrayName + "[" + item1->getName() + "][" + item2->getName() + "]" + " = " + item3->getName() + "\n";
        }
    } else if (this->kind == useArrayKind) {
        if (this->item2 == placeHolderQuaItem) { //Tn = a[1-2]
            return this->item3->getName() + " = " + arrayName + "[" + this->item1->getName() + "]" + "\n";
        } else {
            return this->item3->getName() + " = " + arrayName +
                "[" + this->item1->getName() + "][" + this->item2->getName() + "]" + "\n";
        }
    } else if (this->kind == getReturnValueKind) {
        return this->item3->getName() + " = " + this->item1->getName() + "_call" + "\n";
    } else if (this->kind == mainEntranceKind) {
        return "main_Entrance\n";
    } else if (this->kind == funcCallKind) {
        return "call_" + this->item1->getName() + " + [init]" + "\n";
    } else if (this->kind == funcDefKind) {
        return "define_" + this->item1->getName() + "[init]" + "\n";
    } else if (this->kind == paraDefKind) {
        string token = (this->item3->getType() == intType) ? "int" : "char";
        return "paraDef " + token + " " + this->item3->getName() + " offset " + this->item2->getName() + "\n";
    } else if (this->kind == paraInputKind) {
        string token = (this->item3->getType() == intType) ? "int" : "char";
        return "paraInput " + token + " " + this->item3->getName() + " offset " + this->item2->getName() + "\n";
    } else if (this->kind == mainReturnKind) {
        return "mainReturn\n";
    } else if (this->kind == retFuncReturn) {
        string token = (this->item3->getType() == intType) ? "int" : "char";
        return "return " + token + " " + this->item3->getName() + "\n";
    } else if (this->kind == noRetFuncReturn) {
        return "noRetFuncReturn\n";
    } else if (this->kind == funcEndKind) {
        return "[force]funcEnd: " + this->arrayName + "\n";
    } else if (this->kind == jalKind) {
        return "jal to " + this->item3->getName() + "\n";
    } else if (this->kind == funcCallHeadMarkKind) {
        return this->item1->getName() + "_funcCallHeadMark\n";
    }
    else
        return "toString not implemented\n";
}

Type quadruple::getArrayType() {
    return this->arrayType;
}

void quadruple::generateCode(FILE *mipsOUT, bool isGlobalOrMain) {
    if (this->kind == calculateKind) {
        if (this->item1->getType() == constInt || this->item1->getType() == constChar) {
            immToRegister(8, this->item1->getName(), mipsOUT);
        } else {
            loadToRegister(8, getAddress(this, 1, isGlobalOrMain), mipsOUT);
        }
        if (this->item2->getType() == constInt || this->item2->getType() == constChar) {
            immToRegister(9, this->item2->getName(), mipsOUT);
        } else {
            loadToRegister(9, getAddress(this, 2, isGlobalOrMain), mipsOUT);
        }
        if (this->op == "+") {
            fprintf(mipsOUT, "\taddu $10, $8, $9\n");
        } else if (this->op == "-") {
            fprintf(mipsOUT, "\tsubu $10, $8, $9\n");
        } else if (this->op == "*") {
            fprintf(mipsOUT, "\tmult $8, $9\n");
            fprintf(mipsOUT, "\tmflo $10\n");
        } else if (this->op == "/") {
            fprintf(mipsOUT, "\tdiv $8, $9\n");
            fprintf(mipsOUT, "\tmflo $10\n");
        } else {
            printf("in generateCode, wrong op\n");
        }
        saveToMem(10, getAddress(this, 3, isGlobalOrMain), mipsOUT);
    } else if (this->kind == printfKind) {
        if (this->item1->getType() != constString) { //printf(<expression>)
            Type thisType = this->item1->getType();
            if (thisType == intType) {
                string address = getAddress(this, 1, isGlobalOrMain); //item1
                printInt(address, mipsOUT);
            } else if (thisType == charType) {
                string address = getAddress(this, 1, isGlobalOrMain); //item1
                printChar(address, mipsOUT);
            } else if (thisType == constInt)
                printConstInt(this->getItem1()->getName(), mipsOUT);
            else if (thisType == constChar)
                printConstChar(this->item1->getName(), mipsOUT);
            else
                printf("in quadruple::generateCode---printfKind, what type??\n");
        } else {
            string address = getAddress(this, 1, isGlobalOrMain); //item1
            printString(address, mipsOUT);
            if (this->item2 != nullptr) { //<string>, [<expression>]
                Type thisType = this->item2->getType();
                string address = getAddress(this, 2, isGlobalOrMain); //item1
                if (thisType == intType)
                    printInt(address, mipsOUT);
                else if (thisType == charType)
                    printChar(address, mipsOUT);
                else if (thisType == constInt)
                    printConstInt(this->item2->getName(), mipsOUT);
                else if (thisType == constChar)
                    printConstChar(this->item2->getName(), mipsOUT);
                else
                    printf("in quadruple::generateCode---printfKind, what type??\n");
            }
        }
        printEnter(mipsOUT);
    } else if (this->kind == scanfKind) {
        Type thisType = this->item1->getType();
        string address = getAddress(this, 1, isGlobalOrMain); //item1
        if (thisType == intType) {
            readInt(address, mipsOUT);
        } else {
            readChar(address, mipsOUT);
        }
    } else if (this->kind == constIntDefinition || this->kind == constCharDefinition ||
                this->kind == varIntDefinition || this->kind == varCharDefinition) {
        if (this->item2 != nullptr) {
            string address = getAddress(this, 3, isGlobalOrMain); //assignSpace
            loadConstToRegister(8, this->item2->getName(), mipsOUT);
            saveToMem(8, address, mipsOUT);
        }
    } else if (this->kind == assignKind) {
        if (this->item2->getType() == constInt || this->item2->getType() == constChar) {
            immToRegister(8, this->item2->getName(), mipsOUT);
        } else {
            string address1 = getAddress(this, 2, isGlobalOrMain);
            loadToRegister(8, address1, mipsOUT);
        }
        if (this->item3->getRowNum() != -1 || this->item3->getColNum() != -1) {
            int address = get_num_address(this, 3);
            if (isGlobalOrMain) {
                fprintf(mipsOUT, "\tsw $8, %s\n", to_string(address).c_str());
            } else {
                fprintf(mipsOUT, "\tsw $8, %s($sp)\n", to_string(address).c_str());
            }
        } else {
            string address3 = getAddress(this, 3, isGlobalOrMain);
            saveToMem(8, address3, mipsOUT);
        }
    } else if (this->kind == branchKind) {
        if (this->item1->getType() == constInt || this->item1->getType() == constChar) {
            immToRegister(8, this->item1->getName(), mipsOUT);
        } else {
            loadToRegister(8, getAddress(this, 1, isGlobalOrMain), mipsOUT);
        }
        if (this->item2->getType() == constInt || this->item2->getType() == constChar) {
            immToRegister(9, this->item2->getName(), mipsOUT);
        } else {
            loadToRegister(9, getAddress(this, 2, isGlobalOrMain), mipsOUT);
        }
        fprintf(mipsOUT, "\t%s $8, $9, %s\n", this->op.c_str(), this->item3->getName().c_str());
    } else if (this->kind == jumpKind) {
        fprintf(mipsOUT, "\t%s", this->toString().c_str());
    } else if (this->kind == labelKind) {
        fprintf(mipsOUT, "%s", this->toString().c_str());
    } else if (this->kind == intArrayDefinition || this->kind == charArrayDefinition) {
        string address = getAddress(this, 1, isGlobalOrMain); //看看有没有分配上
    } else if (this->kind == assignArrayKind) {
        if (this->item2 == placeHolderQuaItem) { //a[1] = t
            if (this->item1->getType() == constInt || this->item1->getType() == constChar) {
                immToRegister(8, this->item1->getName(), mipsOUT);
            } else {
                loadToRegister(8, getAddress(this, 1, isGlobalOrMain), mipsOUT);
            }
            fprintf(mipsOUT, "\tsll $8, $8, 2\n"); //offset in $8
            string baseAddress = getBaseAddressByName(arrayName);
            immToRegister(9, baseAddress, mipsOUT);
            fprintf(mipsOUT, "\taddu $8, $8, $9\n");
            if (this->item3->getType() == constInt || this->item3->getType() == constChar) {
                immToRegister(10, this->item3->getName(), mipsOUT); //value
            } else {
                loadToRegister(10, getAddress(this, 3, isGlobalOrMain), mipsOUT);
            }
            if (isGlobalOrMain) {
                fprintf(mipsOUT, "\tsw $10, 0($8)\n");
            } else {
                if (found_in_function_map(arrayName) == 1) {
                    fprintf(mipsOUT, "\taddu $8, $8, $sp\n");
                    fprintf(mipsOUT, "\tsw $10, 0($8)\n");
                } else { //in function, bur found it in globalMap
                    fprintf(mipsOUT, "\tsw $10, 0($8)\n");
                }
            }
        } else {
            int row_num = 0;
            int col_num = 0;
            quaItem *thisTemplate = getTemplate(arrayName);
            row_num = thisTemplate->getRowNum();
            col_num = thisTemplate->getColNum();
            if (this->item1->getType() == constInt || this->item1->getType() == constChar) {
                immToRegister(8, this->item1->getName(), mipsOUT);
            } else {
                loadToRegister(8, getAddress(this, 1, isGlobalOrMain), mipsOUT);
            }
            immToRegister(9, to_string(col_num), mipsOUT);
            fprintf(mipsOUT, "\tmult $8, $9\n");
            fprintf(mipsOUT, "\tmflo $8\n");
            if (this->item2->getType() == constInt || this->item2->getType() == constChar) {
                immToRegister(9, this->item2->getName(), mipsOUT);
            } else {
                loadToRegister(9, getAddress(this, 2, isGlobalOrMain), mipsOUT);
            }
            fprintf(mipsOUT, "\taddu $8, $8, $9\n");
            fprintf(mipsOUT, "\tsll $8, $8, 2\n"); //offset in $8
            if (this->item3->getType() == constInt || this->item3->getType() == constChar) {
                immToRegister(10, this->item3->getName(), mipsOUT); //value
            } else {
                loadToRegister(10, getAddress(this, 3, isGlobalOrMain), mipsOUT);
            }
            string baseAddress = getBaseAddressByName(arrayName);
            immToRegister(9, baseAddress, mipsOUT);
            fprintf(mipsOUT, "\taddu $8, $8, $9\n");
            if (isGlobalOrMain) { // in main
                fprintf(mipsOUT, "\tsw $10, 0($8)\n");
            } else { // in function
                if (found_in_function_map(arrayName) == 1) {
                    fprintf(mipsOUT, "\taddu $8, $8, $sp\n");
                    fprintf(mipsOUT, "\tsw $10, 0($8)\n");
                } else { //in function, bur found it in globalMap
                    fprintf(mipsOUT, "\tsw $10, 0($8)\n");
                }
            }

        }
    } else if (this->kind == useArrayKind) {
        if (this->item2 == placeHolderQuaItem) { //Tn = a[1-2]
            if (this->item1->getType() == constInt || this->item1->getType() == constChar) {
                immToRegister(8, this->item1->getName(), mipsOUT);
            } else {
                loadToRegister(8, getAddress(this, 1, isGlobalOrMain), mipsOUT);
            }
            fprintf(mipsOUT, "\tsll $8, $8, 2\n"); //offset in $8
            string baseAddress = getBaseAddressByName(arrayName);
            immToRegister(9, baseAddress, mipsOUT);
            fprintf(mipsOUT, "\taddu $8, $8, $9\n");
            if (isGlobalOrMain) {
                fprintf(mipsOUT, "\tlw $10, 0($8)\n");
            } else {
                if (found_in_function_map(arrayName) == 1) {
                    fprintf(mipsOUT, "\taddu $8, $8, $sp\n");
                    fprintf(mipsOUT, "\tlw $10, 0($8)\n");
                } else { //in function, bur found it in globalMap
                    fprintf(mipsOUT, "\tlw $10, 0($8)\n");
                }
            } //value in $10
            saveToMem(10, getAddress(this, 3, isGlobalOrMain), mipsOUT);
        } else {
            int row_num = 0;
            int col_num = 0;
            quaItem *thisTemplate = getTemplate(arrayName);
            row_num = thisTemplate->getRowNum();
            col_num = thisTemplate->getColNum();
            if (this->item1->getType() == constInt || this->item1->getType() == constChar) {
                immToRegister(8, this->item1->getName(), mipsOUT);
            } else {
                loadToRegister(8, getAddress(this, 1, isGlobalOrMain), mipsOUT);
            }
            immToRegister(9, to_string(col_num), mipsOUT);
            fprintf(mipsOUT, "\tmult $8, $9\n");
            fprintf(mipsOUT, "\tmflo $8\n");
            if (this->item2->getType() == constInt || this->item2->getType() == constChar) {
                immToRegister(9, this->item2->getName(), mipsOUT);
            } else {
                loadToRegister(9, getAddress(this, 2, isGlobalOrMain), mipsOUT);
            }
            fprintf(mipsOUT, "\taddu $8, $8, $9\n");
            fprintf(mipsOUT, "\tsll $8, $8, 2\n"); //offset in $8
            string baseAddress = getBaseAddressByName(arrayName);
            immToRegister(9, baseAddress, mipsOUT);
            fprintf(mipsOUT, "\taddu $8, $8, $9\n");
            if (isGlobalOrMain) {
                fprintf(mipsOUT, "\tlw $10, 0($8)\n");
            } else {
                if (found_in_function_map(arrayName) == 1) {
                    fprintf(mipsOUT, "\taddu $8, $8, $sp\n");
                    fprintf(mipsOUT, "\tlw $10, 0($8)\n");
                } else { //in function, bur found it in globalMap
                    fprintf(mipsOUT, "\tlw $10, 0($8)\n");
                }
            } //value in $10
            saveToMem(10, getAddress(this, 3, isGlobalOrMain), mipsOUT);
        }
    } else if (this->kind == getReturnValueKind) {
        saveToMem(4, getAddress(this, 3, isGlobalOrMain), mipsOUT);
    } else if (this->kind == mainEntranceKind) {
        fprintf(mipsOUT, "\nmain_Entrance:\n");
    } else if (this->kind == funcCallKind) {
        int moveSum = 0;
        for (int i = 0; i < spaceStack.size(); i++)
            moveSum += spaceStack[i];
        //int space = get_space_by_func_name(this->item3->getName()); its space in spaceStack now
        fprintf(mipsOUT, "\tsubu $sp, $sp, %s\n", to_string(moveSum).c_str());
        immToRegister(9, to_string(moveSum), mipsOUT);
        fprintf(mipsOUT, "\tsw $9, 0($sp)\n");
        spaceStack.pop_back();
    } else if (this->kind == funcCallHeadMarkKind) {
        int space = get_space_by_func_name(this->item1->getName());
        spaceStack.push_back(space);
    } else if (this->kind == funcDefKind) {
        fprintf(mipsOUT, "\n%s:\n", this->item3->getName().c_str());
        fprintf(mipsOUT, "\tsw $ra, 4($sp)\n");
    } else if (this->kind == paraDefKind) {
        //string token = (this->item3->getType() == intType) ? "int" : "char";
        //return "paraDef " + token + " " + this->item3->getName() + " offset " + this->item2->getName() + "\n";
    } else if (this->kind == paraInputKind) {
        if (this->item1->getType() == constInt || this->item1->getType() == constChar) {
            immToRegister(8, this->item1->getName(), mipsOUT);
        } else {
            loadToRegister(8, getAddress(this, 1, isGlobalOrMain), mipsOUT);
        }
        int offset = string2num(this->item2->getName().c_str());
        for (int i = 0; i < spaceStack.size(); i++)
            offset = offset - spaceStack[i];
        string toOldSpOffset = to_string(offset);
        fprintf(mipsOUT, "\tsw $8, %s($sp)\n", toOldSpOffset.c_str());
    } else if (this->kind == mainReturnKind) {
        fprintf(mipsOUT, "\tj mainEnd\n");
    } else if (this->kind == retFuncReturn) {
        if (this->item1->getType() == constInt || this->item1->getType() == constChar) {
            immToRegister(4, this->item1->getName(), mipsOUT);
        } else {
            loadToRegister(4, getAddress(this, 1, isGlobalOrMain), mipsOUT);
        }
        fprintf(mipsOUT, "\tlw $ra, 4($sp)\n");
        fprintf(mipsOUT, "\tlw $8, 0($sp)\n");
        fprintf(mipsOUT, "\taddu $sp, $sp, $8\n");
        fprintf(mipsOUT, "\tjr $ra\n");
    } else if (this->kind == noRetFuncReturn) {
        fprintf(mipsOUT, "\tli $a0, 0x0f0f0f0f\n");
        fprintf(mipsOUT, "\tlw $ra, 4($sp)\n");
        fprintf(mipsOUT, "\tlw $8, 0($sp)\n");
        fprintf(mipsOUT, "\taddu $sp, $sp, $8\n");
        fprintf(mipsOUT, "\tjr $ra\n");
    } else if (this->kind == funcEndKind) {
        fprintf(mipsOUT, "\tli $a0, 0xf0f0f0f0\n");
        fprintf(mipsOUT, "\tlw $ra, 4($sp)\n");
        fprintf(mipsOUT, "\tlw $8, 0($sp)\n");
        fprintf(mipsOUT, "\taddu $sp, $sp, $8\n");
        fprintf(mipsOUT, "\tjr $ra\n");
    }  else if (this->kind == jalKind) {
        fprintf(mipsOUT, "\tjal %s\n", this->item3->getName().c_str());
    }
    else
        printf("this kind never seen, in quadruple::generateCode()\n");
}

void immToRegister(int registerNo, string value, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tli $%d, %s\n", registerNo, value.c_str());
}

void loadToRegister(int registerNo, string address, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tlw $%d, %s\n", registerNo, address.c_str());
}

void saveToMem(int registerNo, string address, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tsw $%d, %s\n", registerNo, address.c_str());
}

void loadConstToRegister(int registerNo, string value, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tli $%d, %s\n", registerNo, value.c_str());
}

void printConstInt(string value, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tli $a0, %s\n", value.c_str());
    fprintf(mipsOUT, "\tli $v0, 1\n");
    fprintf(mipsOUT, "\tsyscall\n");
}

void printConstChar(string value, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tli $a0, %s\n", value.c_str());
    fprintf(mipsOUT, "\tli $v0, 11\n");
    fprintf(mipsOUT, "\tsyscall\n");
}

void printInt(string address, FILE *mipsOUT) {
    loadToRegister(4, address, mipsOUT); //$a0 = value
    fprintf(mipsOUT, "\tli $v0, 1\n");
    fprintf(mipsOUT, "\tsyscall\n");
}

void printChar(string address, FILE *mipsOUT) {
    loadToRegister(4, address, mipsOUT); //$a0 = value
    fprintf(mipsOUT, "\tli $v0, 11\n");
    fprintf(mipsOUT, "\tsyscall\n");
}

void printString (string address, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tla $a0, %s\n", address.c_str());
    fprintf(mipsOUT, "\tli $v0, 4\n");
    fprintf(mipsOUT, "\tsyscall\n");
}

void printEnter(FILE *mipsOUT) {
    fprintf(mipsOUT, "\tla $a0, enter\n");
    fprintf(mipsOUT, "\tli $v0, 4\n");
    fprintf(mipsOUT, "\tsyscall\n");
}

void readInt(string address, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tli $v0, 5\n");
    fprintf(mipsOUT, "\tsyscall\n");
    saveToMem(2, address, mipsOUT);
}

void readChar(string address, FILE *mipsOUT) {
    fprintf(mipsOUT, "\tli $v0, 12\n");
    fprintf(mipsOUT, "\tsyscall\n");
    saveToMem(2, address, mipsOUT);
}

int getValue(quaItem *ptr) {
    if (ptr->getType() == constInt) {
        return string2num(ptr->getName());
    } else if (ptr->getType() == constChar) {
        return char2num(ptr->getName());
    }
    printf("in getValue, wrong type\n");
    return 0;
}

int string2num(string str) {
    int i = 0;
    int negFlag = 1;
    int value = 0;
    if (str[0] == '-') {
        negFlag = -1;
        i++;
    }
    for (i; i < str.length(); i++)
        value = value * 10 + (str[i] - '0');
    return value * negFlag;
}

int char2num(string str) {
    return str[1];
}





