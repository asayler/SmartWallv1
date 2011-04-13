# Andy Sayler
# SmartWall Project
# SmartWall Slave Outlet Sim
# makefile
# Created 2/2/11
#
# Change Log:
# 02/02/11 - Created
# 02/14/11 - Updated for auto generated dependencies
# ---

CC ?= gcc
CFLAGS ?= -g -Wall -Wextra

OBJS = slaveOutlet.o swOutlet.o
EXEC = slaveOutlet

.PHONY : all
all : $(OBJS) $(EXEC)

slaveOutlet : slaveOutlet.o swOutlet.o ../com/SmartWall.o ../com/SmartWallSockets.o ../com/comTools.o ../com/comTools.o
	$(CC) $(CFLAGS) slaveOutlet.o swOutlet.o ../com/SmartWall.o ../com/SmartWallSockets.o ../com/comTools.o ../com/comPrint.o -o slaveOutlet

slaveOutlet.o : slaveOutlet.c
	$(CC) $(CFLAGS) -c slaveOutlet.c 
-include slaveOutlet.d

swOutlet.o : swOutlet.c
	$(CC) $(CFLAGS) -c swOutlet.c
-include swOutlet.d

# Automatically generate dependencies for .c files
%.d : %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY : clean
clean :
	rm -f $(EXEC) $(OBJS)
	rm -f *.d
	rm -f *~
	rm -f core*
