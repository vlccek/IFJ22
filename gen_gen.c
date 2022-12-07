/**
 * @file gen_gen.c
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief Generator of final code
 * Implementation IFJ22 compiler
 */

#include "gen_gen.h"

void generateHeader() {
    printf(".IFJcode22\n"
           "DEFVAR GF@type\n"
           "DEFVAR GF@strlen\n"
           "DEFVAR GF@i\n"
           "DEFVAR GF@j");
}
void generateFooter() {
    printf("# zabudované funkce read_\n"
           "LABEL $BEGIN_readi\n"
           "READ TF@$return int\n"
           "RETURN\n"
           "LABEL $END_readi\n"
           "LABEL $BEGIN_reads\n"
           "READ TF@$return string\n"
           "RETURN\n"
           "LABEL $END_reads\n"
           "LABEL $BEGIN_readf\n"
           "READ TF@$return float\n"
           "RETURN\n"
           "LABEL $END_readf\n"
           "\n"
           "#zabudované funkce převod typů\n"
           "\n"
           "LABEL $BEGIN_intval\n"
           "TYPE GF@type TF@$param0\n"
           "JUMPIFEQ $intval_float GF@type string@float\n"
           "JUMPIFEQ $intval_int GF@type string@int\n"
           "JUMPIFEQ $intval_nil GF@type string@nil\n"
           "# nelze převést\n"
           "EXIT int@7\n"
           "RETURN\n"
           "\n"
           "LABEL $intval_float\n"
           "FLOAT2INT TF@$return TF@$param0\n"
           "RETURN\n"
           "\n"
           "LABEL $intval_int\n"
           "MOVE TF@$return TF@$param0\n"
           "RETURN\n"
           "\n"
           "LABEL $intval_nil\n"
           "MOVE TF@$return int@0\n"
           "RETURN\n"
           "\n"
           "LABEL $END_intval\n"
           "\n"
           "\n"
           "\n"
           "# převod na float\n"
           "LABEL $BEGIN_floatval\n"
           "TYPE GF@type TF@$param0\n"
           "JUMPIFEQ $floatval_float GF@type string@float\n"
           "JUMPIFEQ $floatval_int GF@type string@int\n"
           "JUMPIFEQ $floatval_nil GF@type string@nil\n"
           "# nelze převést\n"
           "EXIT int@7\n"
           "RETURN\n"
           "\n"
           "LABEL $floatval_float\n"
           "MOVE TF@$return TF@$param0\n"
           "RETURN\n"
           "\n"
           "LABEL $floatval_int\n"
           "INT2FLOAT TF@$return TF@$param0\n"
           "RETURN\n"
           "\n"
           "LABEL $floatval_nil\n"
           "INT2FLOAT TF@$return int@0\n"
           "RETURN\n"
           "\n"
           "LABEL $END_floatval\n"
           "\n"
           "# převod na string\n"
           "LABEL $BEGIN_strval\n"
           "TYPE GF@type TF@$param0\n"
           "JUMPIFEQ $strval_string GF@type string@string\n"
           "JUMPIFEQ $strval_nil GF@type string@nil\n"
           "# nelze převést\n"
           "EXIT int@7\n"
           "RETURN\n"
           "\n"
           "LABEL $strval_string\n"
           "MOVE TF@$return TF@$param0\n"
           "RETURN\n"
           "\n"
           "LABEL $strval_nil\n"
           "MOVE TF@$return string@\n"
           "RETURN\n"
           "\n"
           "LABEL $END_strval\n"
           "\n"
           "# zabudovaná funkce strlen\n"
           "LABEL $BEGIN_strlen\n"
           "STRLEN TF@$return TF@$param0\n"
           "RETURN\n"
           "LABEL $END_strlen\n"
           "\n"
           "# zabudovaná funkce chr\n"
           "LABEL $BEGIN_chr\n"
           "INT2CHAR TF@$return TF@$param0\n"
           "RETURN\n"
           "LABEL $END_chr\n"
           "\n"
           "\n"
           "# zabudovaná funkce ord\n"
           "LABEL $BEGIN_ord\n"
           "STRI2INT TF@$return TF@$param0 int@0\n"
           "RETURN\n"
           "LABEL $END_ord\n"
           "# zabudovaná funkce substring\n"
           "LABEL $BEGIN_substring\n"
           "# GF@strlen obsahuje délku stringu\n"
           "STRLEN GF@strlen TF@$param0\n"
           "MOVE GF@i TF@$param1\n"
           "MOVE GF@j TF@$param2\n"
           "JUMPIFEQ $substring_nil GF@i GF@strlen\n"
           "GT GF@type GF@i GF@j\n"
           "JUMPIFEQ $substring_nil GF@type bool@true\n"
           "LT GF@type GF@i int@0\n"
           "JUMPIFEQ $substring_nil GF@type bool@true\n"
           "LT GF@type GF@i int@0\n"
           "JUMPIFEQ $substring_nil GF@type bool@true\n"
           "GT GF@type GF@i GF@strlen\n"
           "JUMPIFEQ $substring_nil GF@type bool@true\n"
           "GT GF@type GF@j GF@strlen\n"
           "JUMPIFEQ $substring_nil GF@type bool@true\n"
           "MOVE TF@$return string@\n"
           "SUB GF@j GF@j int@1\n"
           "LABEL $substring_loop\n"
           "GETCHAR GF@strlen TF@$param0 GF@i\n"
           "CONCAT TF@$return TF@$return GF@strlen\n"
           "ADD GF@i GF@i int@1\n"
           "GT GF@strlen GF@i GF@j\n"
           "JUMPIFNEQ $substring_loop GF@strlen bool@true\n"
           "RETURN\n"
           "\n"
           "LABEL $substring_nil\n"
           "MOVE TF@$return nil@nil\n"
           "RETURN\n"
           "\n"
           "LABEL $END_substring");
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
    else if (symb.dataType == nil)
        sprintf(buf, "nil@nil");
    else if (symb.dataType == booltype) {
        sprintf(buf, symb.token.data.valueInteger == 1 ? "bool@true" : "bool@false");
    }
    return buf;
}

char *generateArgSymVar(symbol_t symb, char *buf) {
    if (symb.type == variable) {
        sprintf(buf, "TF@%s", symb.identifier);
        return buf;
    }
    return generateArgSymbol(symb, buf);
}
char *generateArgSymVarLF(symbol_t symb, char *buf) {
    if (symb.type == variable) {
        sprintf(buf, "LF@%s", symb.identifier);
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
void generateCompareOrEquals(bool greater) {
    printf("POPS GF@j\n"
           "POPS GF@i\n"
           "EQ GF@type GF@i GF@j\n"
           "PUSHS GF@type\n");
    printf("%s GF@type GF@i GF@j\n", greater ? "GT" : "LT");
    printf("PUSHS GF@type\n"
           "ORS");
}
void generateMoveSpecial(i3Instruction_t instruction, bool param) {
    char buf[2048];
    if (param) {
        printf("MOVE TF@%s %s",
               instruction.dest.identifier,
               generateArgSymVarLF(instruction.arg1, buf));
    } else {
        printf("MOVE %s TF@%s",
               generateArgSymVarLF(instruction.dest, buf),
               instruction.arg1.identifier);
    }
}
void generateConcats() {
    printf("POPS GF@j\n"
           "POPS GF@i\n"
           "CONCAT GF@strlen GF@i GF@j\n"
           "PUSHS GF@strlen");
}
void generateLabel(const char *label) {
    printf("\nLABEL %s\n", label);
}
void generateJump(const char *label) {
    printf("JUMP %s\n", label);
}
void generateCall(const char *function_name) {
    printf("CALL $BEGIN_%s\n", function_name);
}

void generateJumps(i3Instruction_t instruction) {
    printf("JUMPIFNEQS %s", instruction.arg1.identifier);
}

void generateInstruction(i3Instruction_t instruction) {
    switch (instruction.type) {
        case I_NOOP:
            printlog("noop");
            break;
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
        case I_CONCATS:
            generateConcats();
            break;
        case I_EQ:
            break;
        case I_NEQ:
            break;
        case I_LT:
            break;
        case I_GT:
            break;
        case I_GTS:
            generateSimpleIns("GTS");
            break;
        case I_LT_OR_EQ:
            generateCompareOrEquals(false);
            break;
        case I_GT_OR_EQ:
            generateCompareOrEquals(true);
            break;
        case I_LTS:
            generateSimpleIns("LTS");
            break;
        case I_EQS:
            generateSimpleIns("EQS");
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
        case I_MOVE_PARAM:
            generateMoveSpecial(instruction, true);
            break;
        case I_MOVE_RETURN:
            generateMoveSpecial(instruction, false);
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
            generateLabel(instruction.arg1.identifier);
            break;
        case I_JUMP:
            generateJump(instruction.arg1.identifier);
            break;
        case I_JUMP_IF_TRUE:
            break;
        case I_JUMP_IF_FALSE:
            break;
        case I_JUMPS_NEQ:
            generateJumps(instruction);
            break;
        case I_CALL:
            generateCall(instruction.arg1.identifier);
            break;
        case I_RETURN:
            generateSimpleIns("RETURN");
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
        case I_POPFRAME:
            generateSimpleIns("POPFRAME");
            break;
        case I_PUSHFRAME:
            generateSimpleIns("PUSHFRAME");
            break;
        case I_CREATEFRAME:
            generateSimpleIns("CREATEFRAME");
            break;
        case I_CONCAT:
            break;
        case I_JUMPS_EQ:
            break;
    }
    printf("\n");
}


void generateExit(int code) {
    if (code < 0 || code > 49) {
        loging("Invalid exit code (%d), will throw error 57 when run", code);
    }
    printf("EXIT int@%d\n\n", code);
}

void generateInstructionArray(i3InstructionArray_t array, symtable_t *symtable) {
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
        generateSimpleIns("RETURN");
        generateLabel(buffer);
    } else {
        generateExit(0);
    }
    loging("Vygenerovana funkce: %s\n", array.functionName);
}


void generate(i3Table_t program, symtable_t symtable) {
    generateHeader();
    printf("\n");
    printf("CREATEFRAME\n");
    for (int i = 0; i < MAX_HTSIZE; ++i) {
        generateInstructionArray(program[i], &symtable);
    }
    generateLabel("$PROGRAM_END");
    generateFooter();
}
