# Andy Sayler
# SmartWall Project
# SmartWall Linux Project
# makefile
# Created 2/2/11
#
# Change Log:
# 02/02/11 - Created
# ---

CC = gcc
CFLAGS = -g -Wall -Wextra

OBJS = slave/slaveOutlet.o master/master.o com/SmartWall.o
EXEC = slave/slaveOutlet master/master

all : slaveOutlet master

slaveOutlet : slave/slaveOutlet.o com/SmartWall.o
	$(CC) $(CFLAGS) slave/slaveOutlet.o com/SmartWall.o -o slave/slaveOutlet

slaveOutlet.o : slave/slaveOutlet.c slave/swOutlet.h com/SmartWall.h
	$(CC) $(CFLAGS) -c slave/slaveOutlet.c 

master : master/master.o
	$(CC) $(CFLAGS) master/master.o -o master/master

master.o : master/master.c com/SmartWall.h
	$(CC) $(CFLAGS) -c master/master.c 

SmartWall.o : com/SmartWall.c com/SmartWall.h
	$(CC) $(CFLAGS) -c com/SmartWall.c 

.PHONY : clean
clean :
	rm -f $(EXEC) $(OBJS)
	rm -f *~
	rm -f slave/*~
	rm -f master/*~
	rm -f com/*~
	rm -f core*
