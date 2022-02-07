NAME = wordcount
CC = gcc
all: $(NAME).o
	$(CC) -o $(NAME).out $(NAME).o
$(NAME).o: $(NAME).c
	$(CC) -c $(NAME).c
clean:
	rm *.o *.out

