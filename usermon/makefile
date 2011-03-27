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

OBJS = usermon.o swChnMsg.o swls.o
EXEC = swChnMsg swls

.PHONY : all
all : swChnMsg swls

swChnMsg : swChnMsg.o ../com/SmartWall.o ../com/comTools.o ../master/swMaster.o
	$(CC) $(CFLAGS) swChnMsg.o ../com/SmartWall.o ../com/comTools.o ../master/swMaster.o -o swChnMsg

swls : swls.o usermon.o ../master/swMaster.o
	$(CC) $(CFLAGS) swls.o usermon.o ../master/swMaster.o -o swls

swChnMsg.o : swChnMsg.c
	$(CC) $(CFLAGS) -c swChnMsg.c 
-include swChnMsg.d

swls.o : swls.c
	$(CC) $(CFLAGS) -c swls.c
-include swls.d

usermon.o : usermon.c
	$(CC) $(CFLAGS) -c usermon.c
-include usermon.d

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
