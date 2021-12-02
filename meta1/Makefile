EXE = gocompiler
FILENAME = gocompiler.l

exe: ${FILENAME}
	lex ${FILENAME}
	clang -o ${EXE} lex.yy.c 
	./${EXE} -l
clear:
	rm -rf *${EXE}
