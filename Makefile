NAME = ft_traceroute
CC = cc

HEADERS_DIR = ./headers
CFLAGS = -I$(HEADERS_DIR) -Wall -Wextra -Werror
LDFLAGS = -lm

SRCS = ./srcs/main.c \
	   ./srcs/traceroute.c \
	   ./srcs/parse_reply.c \
	   ./srcs/signal_handler.c \
	   ./srcs/display_infos.c \
	   ./srcs/trace_utils.c \
	   ./srcs/handle_ttl_exceeded.c \
	   ./srcs/handle_echo_reply.c

		
OBJS = $(SRCS:.c=.o)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $^ $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	@rm -fr $(OBJS) $(BONUS_OBJS)

fclean : clean
	@rm -fr $(NAME)

re :
	@$(MAKE) fclean
	@$(MAKE) all

.PHONY : all bonus clean fclean re
