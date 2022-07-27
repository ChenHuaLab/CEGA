c1 = gcc -ggdb -w -O2 -mavx -mavx2 -mfma -D_linux
c2 = gcc -ggdb -w -O2 -mavx -mavx2 -mfma -D_linux -D_use_bz2
libs1 = -lm -lpthread -lz
libs2 = -lm -lpthread -lz  -lbz2

c=$(c1)
libs=$(libs1)

objects = myfunction.o myutil.o bio.o optimize.o hka2.o
soft=CEGA-InSel

$(soft) : $(objects)
	$(c) -o $(soft) $(objects) $(libs)

myfunction.o : myfunction.h myfunction.c
	$(c) -c myfunction.c
myutil.o : myutil.h myutil.c
	$(c) -c myutil.c
bio.o : bio.h bio.c
	$(c) -c bio.c
optimize.o : optimize.h optimize.c
	$(c) -c optimize.c
hka2.o : hka2.h hka2.c
	$(c) -c hka2.c

clean :
	rm -f $(soft) $(objects)
