EXE = gocompiler
FILENAME1 = gocompiler.l
FILENAME2 = gocompiler.y
FILENAME3 = struct.c
FILENAME4 = struct.h
FILENAME5 = semantics.c
FILENAME6 = semantics.h
FILENAME7 = code.c
FILENAME8 = code.h

exe: ${FILENAME1} ${FILENAME2} ${FILENAME3} ${FILENAME4} ${FILENAME5} ${FILENAME6} ${FILENAME7} ${FILENAME8}
	lex ${FILENAME1}
	yacc -y -d -t -v -Wno-yacc ${FILENAME2}
	cc -o ${EXE} -ggdb3 y.tab.c lex.yy.c ${FILENAME3} ${FILENAME5} ${FILENAME7}
	./${EXE} < testes/factorial.dgo > output.out
clear:
	rm -rf *${EXE}
	rm -f ${EXE}.zip
zip:
	zip -r ${EXE}.zip ${FILENAME1} ${FILENAME2} ${FILENAME3} ${FILENAME4} ${FILENAME5} ${FILENAME6} ${FILENAME7} ${FILENAME8}
