# Andy Sayler
# SmartWall Project
# SmartWall User Monitor
# makefile
# Created 2/5/11
#
# Change Log:
# 02/05/11 - Created
# ---

CC = gcc
CFLAGS = -g -Wall -Wextra

OBJS = usermon.o testMain.o
EXEC = testMain

all : testMain

testMain : usermon.o testMain.o
	$(CC) $(CFLAGS) usermon.o testMain.o -o testMain

usermon.o : usermon.c usermon.h ../com/SmartWall.h ../master/swMaster.h
	$(CC) $(CFLAGS) -c usermon.c

testMain.o : testMain.c usermon.h
	$(CC) $(CFLAGS) -c testMain.c

.PHONY : clean
clean :
	rm -f $(EXEC) $(OBJS)
	rm -f *~
	rm -f core*
