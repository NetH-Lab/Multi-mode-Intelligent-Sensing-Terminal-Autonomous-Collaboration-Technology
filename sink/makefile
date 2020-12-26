#SINK-OBJ = sink.o SockList.o route.o cJSON.o configure.o fusion.o pdu.o resource.o
SINK-OBJ = sink.o SockList.o route.o configure.o fusion.o pdu.o resource.o

CC = gcc

sink: $(SINK-OBJ)
	$(CC) -o sink $(SINK-OBJ)

sink.o: sink.c SockList.h route.h configure.h fusion.h resource.h
	$(CC) -c sink.c

SockList.o: SockList.c
	$(CC) -c SockList.c

route.o: route.c fusion.h pdu.h resource.h
	$(CC) -c route.c

configure.o: configure.c
	$(CC) -c configure.c

fusion.o: fusion.c route.h
	$(CC) -c fusion.c

pdu.o: pdu.c route.h
	$(CC) -c pdu.c

resource.o: resource.c
	$(CC) -c resource.c
#cJSON.o: cJSON.c
#$(CC) -c cJSON.c

#clean
.PHONY: clean
clean:
	rm sink *.o
