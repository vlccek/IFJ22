/**
* @file generator_postproces.h
* @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
* @author Antonín Jarolím (xjarol06@stud.fit.vutbr.cz)
* @brief Post processing of 3 address instructions
* Implementation IFJ22 compiler
*/


#ifndef IFJ22_GENERATOR_POSTPROCES_H
#define IFJ22_GENERATOR_POSTPROCES_H

#include "generator_3adres.h"
void postprocess(i3Table_t program, symtable_t symtable);

#endif//IFJ22_GENERATOR_POSTPROCES_H
