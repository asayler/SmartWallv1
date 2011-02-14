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

OBJS = slaveOutlet.o
EXEC = slaveOutlet

.PHONY : all
all : slaveOutlet

slaveOutlet : ../com/SmartWall.o slaveOutlet.o
	$(CC) $(CFLAGS) ../com/SmartWall.o slaveOutlet.o -o slaveOutlet

slaveOutlet.o : slaveOutlet.c
	$(CC) $(CFLAGS) -c slaveOutlet.c 
-include slaveOutlet.d

../com/SmartWall.o : ../com/SmartWall.c
	$(CC) $(CFLAGS) -c ../com/SmartWall.c -o ../com/SmartWall.o 
-include ../com/SmartWall.d

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
