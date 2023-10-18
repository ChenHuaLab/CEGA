c1 = gcc -ggdb -w -O2 -mavx -mfma -D_linux
c2 = gcc -ggdb -w -O2 -mavx -mavx2 -mfma -D_linux -D_use_bz2
libs1 = -lm -lpthread -lz
libs2 = -lm -lpthread -lz  -lbz2

c=$(c1)
libs=$(libs1)

objects = myfunc.o bio.o optimize.o hka2.o
objects2 = optimize.o hka2.o
soft=CEGA

$(soft) : $(objects)
	$(c) -o $(soft) $(objects) $(libs)

optimize.o : optimize.h optimize.c
	$(c) -c optimize.c
hka2.o : hka2.h hka2.c
	$(c) -c hka2.c

clean :
	rm -f $(soft) $(objects2)
