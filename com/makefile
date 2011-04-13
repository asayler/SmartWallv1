# Andy Sayler
# SmartWall Project
# SmartWall Common
# makefile
# Created 2/14/11
#
# Change Log:
# 02/14/11 - Created
# ---

CC ?= gcc
CFLAGS ?= -g -Wall -Wextra #-DSWDEBUG

OBJS = swDevice.o SmartWall.o SmartWallSockets.o comTest.o comTools.o comPrint.o
EXEC = comTest

.PHONY : all
all : $(OBJS) $(EXEC)

swDevice.o : swDevice.c
	$(CC) $(CFLAGS) -c swDevice.c 
-include swDevice.d

SmartWall.o : SmartWall.c
	$(CC) $(CFLAGS) -c SmartWall.c 
-include swDevice.d

SmartWallSockets.o : SmartWallSockets.c
	$(CC) $(CFLAGS) -c SmartWallSockets.c 
-include swDevice.d

comTest : comTest.o SmartWall.o comTools.o comPrint.o
	$(CC) $(CFLAGS) comTest.o SmartWall.o comTools.o comPrint.o -o comTest 

comTest.o : comTest.c
	$(CC) $(CFLAGS) -c comTest.c 
-include comTest.d

comTools.o : comTools.c
	$(CC) $(CFLAGS) -c comTools.c 
-include comTools.d

comPrint.o : comPrint.c
	$(CC) $(CFLAGS) -c comPrint.c 
-include comPrint.d

# Automatically generate dependencies for .c files
%.d : %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY : test
test : comTest
	valgrind -q ./comTest -w > writeTestOut
	valgrind -q ./comTest -ref > writeRefOut
	diff writeTestOut writeRefOut
	rm -f writeTestOut
	valgrind -q ./comTest -rw > writeTestOut
	diff writeTestOut writeRefOut
	rm -f writeTestOut writeRefOut

.PHONY : clean
clean :
	rm -f $(OBJS) $(EXEC)
	rm -f *.d
	rm -f *~
	rm -f core*
