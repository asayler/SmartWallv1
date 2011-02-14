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
CFLAGS ?= -g -Wall -Wextra

OBJS = SmartWall.o

.PHONY : all
all : SmartWall.o

SmartWall.o : SmartWall.c
	$(CC) $(CFLAGS) -c SmartWall.c 
-include SmartWall.d

# Automatically generate dependencies for .c files
%.d : %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.PHONY : clean
clean :
	rm -f $(OBJS)
	rm -f *.d
	rm -f *~
	rm -f core*
