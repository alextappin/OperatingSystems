COPTS=-Wall -g -c -O0 -pthread
OBJS=sched.o \
	queue.o \
	list.o \
	mythreadio.o \

EXES=test1 \
	test2 \
	test3 \
	test4 \
	test5 \
	test5b \
	test6 \
	test7 \
	test8 \
	test9 \

all: $(EXES)

clean:
	rm -f $(OBJS)
	rm -f $(EXES)

test1: test1.c $(OBJS)
	gcc $@.c $(OBJS) -g -o $@ -pthread

%: %.c $(OBJS)
	gcc $@.c $(OBJS) -g -o $@ -pthread

.c.o:
	gcc $(COPTS) $? -o $@
