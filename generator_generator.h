/**
 * @file generator_generator.h
 * @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
 * @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
 * @brief Generator of final code
 * Implementation IFJ22 compiler
 */

#ifndef IFJ22_GENERATOR_H
#define IFJ22_GENERATOR_H
#include "common.h"
#include "generator_3adres.h"

void generate(i3Table_t program, symtable_t symtable);

#endif // IFJ22_GENERATOR_H