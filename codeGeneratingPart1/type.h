//
// Created by SAMSUNG on 2020/11/10.
//

#ifndef SPLITERRORDEALING_TYPE_H
#define SPLITERRORDEALING_TYPE_H

enum Type {undefinedType, intType, charType, voidType, funcType, intCharType, constInt,
            constChar, constString, printfType, scanfType};

enum quadrupleKind {calculateKind, printfKind, scanfKind, constIntDefinition, constCharDefinition,
            varIntDefinition, varCharDefinition, assignKind};

enum defineType {defineConstInt, defineConstChar, defineVarInt, defineVarChar};

#endif //SPLITERRORDEALING_TYPE_H
