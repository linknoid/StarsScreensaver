
LIBS= -lscrnsave -lmingw32 -lgdi32 -mno-cygwin -mwindows -Uunix 
GLLIBS = -lopengl32 -lglu32 ${LIBS}
OPTIMIZE= -fbranch-probabilities 
PROFILE= -profile-arcs
FLAGS= -O3 -march=i686 
	#-funsafe-math-optimizations
INCLUDES= -I/usr/include/mingw
DEFINES= -DWIN32 -DHAVE_OPENGL \
		 -DDISABLE_LOGGING
CC= g++ ${FLAGS} ${INCLUDES} ${DEFINES}

LOGHEADER= logging/logging.h 
GLOBALFILES= ${LOGHEADER} consts.h makefile
OBJS= logging/logging.o consts.o starsscr.o starsreg.o RegistrySettings.o GLRenderer.o stars.o screensave.o resources.cof

final: stars.scr
	strip stars.scr


stars.scr: ${OBJS} 
	g++ ${OBJS} ${GLLIBS} -o stars.scr 


stars.o: stars.cpp stars.h starsreg.o StarsRenderer.h ${GLOBALFILES} 
	${CC} -c stars.cpp -o stars.o

GLRenderer.o: GLRenderer.cpp GLRenderer.h StarsRenderer.h ${GLOBALFILES} 
	${CC} -c GLRenderer.cpp -o GLRenderer.o

RegistrySettings.o: RegistrySettings.h RegistrySettings.cpp StarsSettings.h ${GLOBALFILES}
	${CC} -c RegistrySettings.cpp -o RegistrySettings.o

starsreg.o: starsreg.cpp starsreg.h stars.h ${GLOBALFILES} 
	${CC} -c starsreg.cpp -o starsreg.o

starsscr.o: starsscr.cpp starsreg.h GLRenderer.h stars.h ${GLOBALFILES} 
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


