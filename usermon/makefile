# Andy Sayler
# SmartWall Project
# SmartWall User Monitor
# makefile
# Created 2/5/11
#
# Change Log:
# 02/05/11 - Created
# 02/14/11 - Updated for auto generated dependencies
# ---

CC ?= gcc
CFLAGS ?= -g -Wall -Wextra

OBJS = usermon.o testMain.o
EXEC = testMain

.PHONY : all
all : testMain

testMain : usermon.o testMain.o
	$(CC) $(CFLAGS) usermon.o testMain.o -o testMain

usermon.o : usermon.c
	$(CC) $(CFLAGS) -c usermon.c
-include usermon.d

testMain.o : testMain.c
	$(CC) $(CFLAGS) -c testMain.c
-include testMain.d

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
