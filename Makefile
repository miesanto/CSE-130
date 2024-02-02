#------------------------------------------------------------------------------
# Makefile for CSE 130 Assignment 1: Command-line Memory
#
# make all               produces the memory binary
# make clean             removes all .o and binary files
# make memory            produces the memory binary
# make format            clang format
#------------------------------------------------------------------------------

CC             = clang
CFLAGS         = -Wall -Wextra -Werror -pedantic
SRCS		   = $(wildcard *.c)
OBJS           = $(SRCS:.c=.o)


all : memory

memory : $(OBJS)
	clang -o $@ $^

%.o : %.c
	clang $(CFLAGS) -c $<

clean :
	rm -f $(OBJS) memory

format :
	clang-format -i -style=file *.[ch]

scan-build : clean
	scan-build make