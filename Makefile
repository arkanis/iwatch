iwatch: iwatch.c
	gcc -std=c99 -Werror -Wall -Wextra iwatch.c -o iwatch

clean:
	rm -f iwatch