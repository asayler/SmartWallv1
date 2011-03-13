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

OBJS = usermon.o testMain.o swChnMsg.o
EXEC = testMain swChnMsg

.PHONY : all
all : testMain swChnMsg

testMain : usermon.o testMain.o ../master/swMaster.o
	$(CC) $(CFLAGS) usermon.o testMain.o ../master/swMaster.o -o testMain

swChnMsg : swChnMsg.o ../com/SmartWall.o
	$(CC) $(CFLAGS) swChnMsg.o ../com/SmartWall.o -o swChnMsg

usermon.o : usermon.c
	$(CC) $(CFLAGS) -c usermon.c
-include usermon.d

swChnMsg.o : swChnMsg.c
	$(CC) $(CFLAGS) -c swChnMsg.c 
-include swChnMsg.d

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
