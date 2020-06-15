#
# Makefile 
#
# Type  make    to compile all the programs
# in the chapter 
#

# all: 	mirr \
# 		functions \
# 		structures \


# mirr: mirr.c
# 	gcc -g mirr.c functions.c structures.c -o mirr

# functions: functions.c
# 	gcc -g functions.c structures.c -o functions

# structures: structures.c
# 	gcc -g structures.c -o structures


# clean: 
# 	rm -f 	mirr \
# 		functions \
# 		structures \


#
# In order to execute this "Makefile" just type "make"
#	A. Delis (ad@di.uoa.gr)
#

OBJS 	= mirr.o structures.o functions.o
SOURCE	= mirr.c structures.c functions.c
HEADER  = structures.h functions.h
OUT  	= mirr
CC	= gcc
FLAGS   = -g -c
# -g option enables debugging mode
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

# create/compile the individual files >>separately<<
main.o: mirr.c
	$(CC) $(FLAGS) mirr.c

Structures.o: structures.c
	$(CC) $(FLAGS) structures.c

Nodes_Edges.o: functions.c
	$(CC) $(FLAGS) functions.c
# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)