BIN=osmixer
OBJ=${BIN}.o

CC?=cc
CFLAGS?=-O2 -march=native
CPPFLAGS?=`pkgconf --cflags elementary`
LDFLAGS?=
LIBS=`pkgconf --libs elementary`
PREFIX?=/usr/local

all: ${BIN}

${BIN}: ${OBJ}
	${CC} ${LDFLAGS} -o $@ ${OBJ} ${LIBS}

.c.o:
	${CC} -c ${CFLAGS} ${CPPFLAGS} -o $@ $<

install:
	install -m 0755 -d ${DESTDIR}${PREFIX}/bin
	install -m 0555 -s ${BIN} ${DESTDIR}${PREFIX}/bin

clean:
	rm -f ${BIN} *.o ${MAN}

${BIN}.o: ${BIN}.c
