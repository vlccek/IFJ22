all: prekladac
# CPPFLAgs jsou flagy preprocesoru, ne c++ :D
%.o : %.c %.h
		$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

OBJECTS = symtable.o parser.o lex.o main.o LLtable.o dynstring.o expParse.o stack.o common.o queue.o semanticActions.o generator/3adres.o generator/generator.o generator/igen.o

prekladac: $(OBJECTS)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

clean:
	rm -f $(OBJECTS) prekladac
