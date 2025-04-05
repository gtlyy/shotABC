
prefix=/usr
exec_prefix=${prefix}
libdir=${exec_prefix}/lib
includedir=${prefix}/include/SDL2
CC=gcc

Cflags=-I${includedir}
Libs=-L${libdir}  -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2  

shotABC: shotABC.o
	${CC} -Wall -o shotABC shotABC.o ${Libs}

shotABC.o: shotABC.c
	${CC} -Wall -c shotABC.c ${Cflags}

clean: shotABC.o shotABC
	rm -fv shotABC.o shotABC
