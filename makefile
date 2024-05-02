all: VCUBE

VCUBE: VCUBE.o smpl.o rand.o
	$(LINK.c) -o $@ -Bstatic VCUBE.o smpl.o rand.o -lm

smpl.o: smpl.c smpl.h
	$(COMPILE.c)  -g smpl.c

VCUBE.o: VCUBE.c smpl.h
	$(COMPILE.c) -g  VCUBE.c

cisj.o: cisj.c
	$(COMPILE.c) -g cisj.c
rand.o: rand.c
	$(COMPILE.c) -g rand.c

clean:
	$(RM) *.o VCUBE relat saida

