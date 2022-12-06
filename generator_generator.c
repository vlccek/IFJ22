/**
 * @file generator_generator.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief generátor výsledného kódu
 * Implementace překladače jazyka IFJ22
 */

#include "generator_generator.h"

void generateHeader() {
    printf(".IFJcode22\n");
}

const char *convertChar(char c, char convBuffer[51]) {
    sprintf(convBuffer, "\\0%d", c);
    return convBuffer;
}
char *convertString(dynStr_t *dynStr, char *string) {
    char buf[2];
    char convBuffer[51];
    convBuffer[0] = '\0';
    buf[1] = '\0';
    char c;
    while ((c = *string++) != '\0') {
        buf[0] = c;
        if (c < 33 || c == 35 || c == 92)
            dstrAppend(dynStr, convertChar(c, convBuffer));
        else
            dstrAppend(dynStr, buf);
    }
    return dynStr->string;
}

/// Converts any symbol literal to ifjcode literal
/// \param symb symbol to be converted
/// \param buf buffer where output will be stored
/// \return buf parameter
char *generateArgSymbol(symbol_t symb, char *buf) {
    if (symb.type != literal) {
        InternalError("Printed symbol is not literal.");
    }
    if (symb.dataType == string) {
        dynStr_t *string = dstrInit();
        sprintf(buf, "string@%s", convertString(string, symb.token.data.valueString->string));
        dstrFree(string);
    } else if (symb.dataType == integer)
        sprintf(buf, "int@%d", symb.token.data.valueInteger);
    else if (symb.dataType == floating)
        sprintf(buf, "float@%a   # %f", symb.token.data.valueFloat, symb.token.data.valueFloat);
    return buf;
}

char *generateArgSymVar(symbol_t symb, char *buf) {
    if (symb.type == variable) {
        sprintf(buf, "TF@%s", symb.identifier);
        return buf;
    }
    return generateArgSymbol(symb, buf);
}

void generateWrite(i3Instruction_t instruction) {
    char buf[2048];
    printf("WRITE %s", generateArgSymVar(instruction.arg1, buf));
}

void generateDefvar(i3Instruction_t instruction) {
    printf("DEFVAR TF@%s", instruction.arg1.token.data.valueString->string);
}

void generateMove(i3Instruction_t instruction) {
    char buf[2048];
    printf("MOVE TF@%s %s",
           instruction.dest.token.data.valueString->string,
           generateArgSymVar(instruction.arg1, buf));
}

void generatePushs(i3Instruction_t instruction) {
    char buf[2048];
    printf("PUSHS %s",
           generateArgSymVar(instruction.arg1, buf));
}

void generatePops(i3Instruction_t instruction) {
    char buf[2048];// todo: @Kiznoh tvuj napad se mi prestal libit od doby co musim
    // todo: mit takto buffer v kazde funkci, pls fix :-)
    if (instruction.dest.type != variable) {
        printSymbol(&instruction.arg1);
        InternalError("Cannot push from stack to above symbol");
    }

    printf("POPS %s", generateArgSymVar(instruction.dest, buf));
}

void generateSimpleIns(const char *instructionName) {
    printf("%s", instructionName);
}

void generateInstruction(i3Instruction_t instruction) {
    switch (instruction.type) {
        case I_ADDS:
            generateSimpleIns("ADDS");
            break;
        case I_SUBS:
            generateSimpleIns("SUBS");
            break;
        case I_MULS:
            generateSimpleIns("MULS");
            break;
        case I_DIVS:
            generateSimpleIns("DIVS");
            break;
        case I_IDIVS:
            generateSimpleIns("IDIVS");
            break;
        case I_CONCAT:
            break;
        case I_EQ:
            break;
        case I_NEQ:
            break;
        case I_LT:
            break;
        case I_GT:
            break;
        case I_LEQ:
            break;
        case I_GEQ:
            break;
        case I_AND:
            break;
        case I_OR:
            break;
        case I_NOT:
            break;
        case I_MOVE:
            generateMove(instruction);
            break;
        case I_INT2FLOAT:
            break;
        case I_READ:
            break;
        case I_WRITE:
            generateWrite(instruction);
            break;
        case I_STRLEN:
            break;
        case I_LABEL:
            break;
        case I_JUMP:
            break;
        case I_JUMP_IF_TRUE:
            break;
        case I_JUMP_IF_FALSE:
            break;
        case I_CALL:
            break;
        case I_RETURN:
            break;
        case I_DEFVAR:
            generateDefvar(instruction);
            break;
        case I_PUSHS:
            generatePushs(instruction);
            break;
        case I_POPS:
            generatePops(instruction);
            break;
        case I_INT2FLOATS:
            generateSimpleIns("INT2FLOATS");
            break;
        case I_FLOAT2INTS:
            generateSimpleIns("FLOAT2INTS");
            break;
        case I_INT2CHARS:
            generateSimpleIns("INT2CHARS");
            break;
        case I_STRI2INTS:
            generateSimpleIns("STRI2INTS");
            break;
    }
    printf("\n");
}

void generateLabel(char *label) {
    printf("\nLABEL %s\n", label);
}
void generateJump(char *label) {
    printf("JUMP %s\n\n", label);
}
void generateExit(int code) {
    if (code < 0 || code > 49) {
        loging("Invalid exit code (%d), will throw error 57 when run", code);
    }
    printf("EXIT int@%d\n\n", code);
}

void generateInstructionArray(i3InstructionArray_t array) {
    if (array.instructions == NULL)
        return;
    if (strcmp(array.functionName, "$MainBody") != 0) {
        char buffer[2048];
        sprintf(buffer, "$BEGIN_%s", array.functionName);
        generateLabel(buffer);
    }
    for (int i = 0; i < array.size; ++i) {
        generateInstruction(array.instructions[i]);
    }
    if (strcmp(array.functionName, "$MainBody") != 0) {
        char buffer[2048];
        sprintf(buffer, "$END_%s", array.functionName);
        generateLabel(buffer);
    } else {
        generateExit(0);
    }
    loging("Vygenerovana funkce: %s\n", array.functionName);
}


void generate(i3Table_t program) {
    generateHeader();
    printf("\n");
    printf("CREATEFRAME\n");
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        generateInstructionArray(program[i]);
    }
    generateLabel("$PROGRAM_END");
}
