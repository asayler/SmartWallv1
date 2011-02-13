# Andy Sayler
# SmartWall Project
# SmartWall Slave Outlet Sim
# makefile
# Created 2/2/11
#
# Change Log:
# 02/02/11 - Created
# ---

CC = gcc
CFLAGS = -g -Wall -Wextra

OBJS = slaveOutlet.o
EXEC = slaveOutlet

slaveOutlet : slaveOutlet.o
	$(CC) $(CFLAGS) slaveOutlet.o -o slaveOutlet

slaveOutlet.o : slaveOutlet.c swOutlet.h ../com/SmartWall.h
	$(CC) $(CFLAGS) -c slaveOutlet.c 

.PHONY : clean
clean :
	rm -f $(EXEC) $(OBJS)
	rm -f *~
	rm -f core*
