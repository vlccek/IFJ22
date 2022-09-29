lexTest: lex.c lexTest.c
	gcc lexTest.c lex.c dynstring.c -o lexTest -std=c99