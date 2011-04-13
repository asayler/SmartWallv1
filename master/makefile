# Andy Sayler
# SmartWall Project
# SmartWall Master
# makefile
# Created 2/2/11
#
# Change Log:
# 02/02/11 - Created
# 02/14/11 - Updated for auto generated dependencies
# ---

CC ?= gcc
CFLAGS ?= -g -Wall -Wextra

OBJS = swMaster.o master.o masterTest.o
EXEC = master masterTest

.PHONY : all
all : master masterTest swMaster.o

master : master.o swMaster.o
	$(CC) $(CFLAGS) master.o swMaster.o -o master

masterTest : masterTest.o swMaster.o
	$(CC) $(CFLAGS) masterTest.o swMaster.o -o masterTest

swMaster.o : swMaster.c
	$(CC) $(CFLAGS) -c swMaster.c 
-include swMaster.d

masterTest.o : masterTest.c
	$(CC) $(CFLAGS) -c masterTest.c 
-include swMaster.d

master.o : master.c
	$(CC) $(CFLAGS) -c master.c 
-include master.d

# Automatically generate dependencies for .c files
%.d : %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY : test
test : masterTest
	sed '/^$$/d' ../devices.sws > devicesOrig.tmp
	valgrind -q ./masterTest
	sed '/^$$/d' ../devices.sws > devices.tmp
	diff devicesOrig.tmp devices.tmp
	rm -f devicesOrig.tmp devices.tmp

.PHONY : clean
clean :
	rm -f $(EXEC) $(OBJS)
	rm -f *.d
	rm -f *~
	rm -f core*
