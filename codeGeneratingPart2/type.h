//
// Created by SAMSUNG on 2020/11/10.
//

#ifndef SPLITERRORDEALING_TYPE_H
#define SPLITERRORDEALING_TYPE_H

enum Type {undefinedType, intType, charType, voidType, funcType, intCharType, constInt,
            constChar, constString, printfType, scanfType, branchType, labelType, jumpType,
            intArrayType, charArrayType, jalType};

enum quadrupleKind {calculateKind, printfKind, scanfKind, constIntDefinition, constCharDefinition,
            varIntDefinition, varCharDefinition, assignKind, branchKind, labelKind, jumpKind,
            intArrayDefinition, charArrayDefinition, assignArrayKind, useArrayKind, getReturnValueKind,
            mainEntranceKind, funcDefKind, funcCallKind, paraDefKind, paraInputKind,
            mainReturnKind, retFuncReturn, noRetFuncReturn, funcEndKind, undefinedKind, jalKind,
            funcCallHeadMarkKind};

enum defineType {defineConstInt, defineConstChar, defineVarInt, defineVarChar,
                defineIntArray, defineCharArray};

#endif //SPLITERRORDEALING_TYPE_H
