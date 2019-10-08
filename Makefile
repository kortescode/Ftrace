CC			=	gcc
RM			=	rm -f
PRINT		=	printf

FTRACE		=	ftrace
TEST		=	fct_test
DOTFILE		=	graph.dot

CFLAGS		+=	-W -Wall -Wextra -Werror
CFLAGS		+=	-ansi -pedantic
CFLAGS		+=	-I./include/
CFLAGS		+=	-D_BSD_SOURCE -D_XOPEN_SOURCE=700

LDFLAGS		+=	-L./lib/ -lelf
#LDFLAGS	+=	-L/usr/lib/ -lelf

FTRACE_SRCS	=	main.c				\
				xfunctions.c		\
				process_follow.c 	\
				ftrace.c			\
				syscall.c			\
				signal.c			\
				call.c				\
				modrm.c				\
				sib.c				\
				dotty.c				\
				signals_tab.c		\
				syscalls_tab.c		\

TEST_SRCS	=	test/main.c			\

FTRACE_OBJS	=	$(FTRACE_SRCS:.c=.o)

TEST_OBJS	=	$(TEST_SRCS:.c=.o)

all			:	$(FTRACE) $(TEST)

$(FTRACE)	:	$(FTRACE_OBJS)
				$(CC) $(CFLAGS) -o $(FTRACE) $(FTRACE_OBJS) $(LDFLAGS)
				@$(PRINT) "\033[32m*** Compilation of binary $(FTRACE) done ***\n\033[00m"

$(TEST)		:	$(TEST_OBJS)
				$(CC) $(CFLAGS) -o $(TEST) $(TEST_OBJS) $(LDFLAGS)
				@$(PRINT) "\033[32m*** Compilation of binary $(TEST) done ***\n\033[00m"

clean		:
				$(RM) $(FTRACE_OBJS)
				$(RM) $(TEST_OBJS)
				$(RM) *~ \#*\#
				@$(PRINT) "\033[31m*** Objects are removed ***\n\033[00m"

fclean		:	clean
				$(RM) $(DOTFILE)
				$(RM) $(FTRACE)
				@$(PRINT) "\033[31m*** Binary $(FTRACE) is removed ***\n\033[00m"
				$(RM) $(TEST)
				@$(PRINT) "\033[31m*** Binary $(TEST) is removed ***\n\033[00m"

re			:	fclean all

.PHONY		:	all clean fclean re