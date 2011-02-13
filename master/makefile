# Andy Sayler
# SmartWall Project
# SmartWall Master
# makefile
# Created 2/2/11
#
# Change Log:
# 02/02/11 - Created
# ---

CC = gcc
CFLAGS = -g -Wall -Wextra

OBJS = master.o
EXEC = master

master : master.o
	$(CC) $(CFLAGS) master.o -o master

master.o : master.c ../com/SmartWall.h
	$(CC) $(CFLAGS) -c master.c 

.PHONY : clean
clean :
	rm -f $(EXEC) $(OBJS)
	rm -f *~
	rm -f core*
