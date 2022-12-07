##
## @file Makefile
## @brief Makefile pro kompilaci projektu
## @author Jan Brudný (xbrudn02@stud.fit.vutbr.cz)
## Implementation IFJ22 compiler
##
all: prekladac
.PHONY: prekladac %.o tar clean clean_o
# CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic 
# CPPFLAgs jsou flagy preprocesoru
%.o : %.c %.h
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

OBJECTS = symtable.o parser.o lex.o main.o LLtable.o dynstring.o expParse.o stack.o common.o queue.o semanticActions.o generator_3adres.o generator_generator.o generator_igen.o stackIf.o generator_postproces.o

prekladac: $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^
	make clean_o

clean: clean_o
	rm prekladac

clean_o:
	rm -f $(OBJECTS)

tar:
	tar -czvf xjarol06.tgz *.c *.h Makefile dokumentace.pdf
	./is_it_ok.sh xjarol06.tgz testFolder
	@echo "\033[1;31m##### A vypl si exit na InternalErroru?? #####\033[0m"

