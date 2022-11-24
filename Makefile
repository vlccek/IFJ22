all: prekladac
.PHONY: prekladac %.o tar clean
# CPPFLAgs jsou flagy preprocesoru, ne c++ :D
%.o : %.c %.h
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

OBJECTS = symtable.o parser.o lex.o main.o LLtable.o dynstring.o expParse.o stack.o common.o queue.o semanticActions.o generator_3adres.o generator_generator.o generator_igen.o

prekladac: $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

clean:
	rm -f $(OBJECTS) prekladac


tar:
	tar -czvf xjarol06.tgz *.c *.h Makefile dokumentace.pdf
