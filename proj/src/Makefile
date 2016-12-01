# Makefile for Tron

COMPILER_TYPE= gnu

CC= gcc

PROG= Tron
SRCS= Tron.c game.c vbe.c video_gr.c otherlabs.c read_bitmap.c tools.c

CCFLAGS= -Wall

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

BINDIR?= /usr/sbin

.include <bsd.prog.mk>
.include <bsd.gcc.mk>

