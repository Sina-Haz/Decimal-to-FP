all: convert

convert:convert.c
	gcc -g -Wall -Werror -fsanitize=address convert.c -o convert
clean:
	rm convert