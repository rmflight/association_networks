OBJS = anet.o profile.o mapper.o metric.o
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)
BINDIR = ../bin

anet : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o $(BINDIR)/anet

anet.o : anet.cpp anet.h
	$(CC) $(CFLAGS) anet.cpp

profile.o : profile.cpp profile.h
	$(CC) $(CFLAGS) profile.cpp

mapper.o : mapper.cpp mapper.h
	$(CC) $(CFLAGS) mapper.cpp

metric.o : metric.cpp metric.h
	$(CC) $(CFLAGS) metric.cpp

clean:
	rm -f *.o *~ $(BINDIR)/anet
