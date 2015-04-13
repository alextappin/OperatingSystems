COPTS=-Wall -g -c -O0 -pthread

OBJS=listtest.o \
	 sequence.o \
	 list.o \
	 
OBJS2=listtest2.o \
	 sequence.o \
	 list2.o \
	 
all: listtest listtest2

clean:
	rm -f $(OBJS)
	rm -f $(OBJS2)
	rm -f listtest
	rm -f listtest2
	rm -f out
	rm -f out2

.c.o:
	gcc $(COPTS) $? -o $@

listtest: $(OBJS)
	gcc $(OBJS) -g -o listtest -pthread
listtest2: $(OBJS2)
	gcc $(OBJS2) -g -o listtest2 -pthread


