EXE = gocompiler
FILENAME1 = gocompiler.l
FILENAME2 = gocompiler.y

exe: ${FILENAME1} ${FILENAME2}
	lex ${FILENAME1}
	yacc -y -d -t -v ${FILENAME2}
	cc -o ${EXE} y.tab.c lex.yy.c struct.c
	./${EXE} -t < testes/error_unbalanced_par.dgo 
clear:
	rm -rf *${EXE}
	rm -f ${EXE}.zip
zip:
	zip -r ${EXE}.zip ${FILENAME1} ${FILENAME2} struct.c struct.h y.tab.c y.tab.h