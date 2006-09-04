
LIBS= -lscrnsave -lmingw32 -lgdi32 -mno-cygwin -mwindows -Uunix 
GLLIBS = -lopengl32 -lglu32 ${LIBS}
OPTIMIZE= -fbranch-probabilities 
PROFILE= -profile-arcs
FLAGS= -O3 
INCLUDES= -I/usr/include/mingw
DEFINES= -DWIN32 -DHAVE_OPENGL \
		 -DDISABLE_LOGGING
CC= g++ ${FLAGS} ${INCLUDES} ${DEFINES}

LOGHEADER= logging/logging.h 
GLOBALFILES= ${LOGHEADER} consts.h makefile
OBJS= logging/logging.o consts.o starsscr.o starsreg.o glstars.o stars.o screensave.o resources.cof

final: stars.scr
	strip stars.scr


stars.scr: ${OBJS} 
	g++ ${OBJS} ${GLLIBS} -o stars.scr 


stars.o: stars.cpp stars.h ${GLOBALFILES} 
	${CC} -c stars.cpp -o stars.o

glstars.o: glstars.cpp glstars.h stars.h ${GLOBALFILES} 
	${CC} -c glstars.cpp -o glstars.o

starsreg.o: starsreg.cpp starsreg.h glstars.h stars.h ${GLOBALFILES} 
	${CC} -c starsreg.cpp -o starsreg.o

starsscr.o: starsscr.cpp starsreg.h glstars.h stars.h ${GLOBALFILES} 
	${CC} -c starsscr.cpp -o starsscr.o

screensave.o: screensave.cpp screensave.h ${GLOBALFILES}
	${CC} -c screensave.cpp -o screensave.o

consts.o: consts.c consts.h 
	${CC} -c consts.c -o consts.o

resources.cof: stars.ico stars.rc ${GLOBALFILES}
	windres -i stars.rc -o resources.cof -O coff -F pe-i386

logging/logging.o: logging/logging.cpp ${GLOBALFILES} 
	cd logging && ${CC} -c logging.cpp -o logging.o



clean:
	rm -f *~ *.o logging/*.o resources.cof


