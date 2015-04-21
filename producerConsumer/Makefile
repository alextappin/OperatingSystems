COPTS=-Wall -g -c -O0 -pthread

OBJS=queue.o \
	 consumer.o \

all: queue consumer

clean:
	rm -f $(OBJS)
	rm -f queue
	rm -f consumer

.c.o:
	gcc $(COPTS) $? -o $@

queue: $(OBJS)
	gcc $(OBJS) -g -o queue -pthread

consumer: $(OBJS)
	gcc $(OBJS) -g -o consumer -pthread