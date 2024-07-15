all: clean Server Client main

clean:
	rm -f Server Client Initializer

clear:
	rm -f Server Client Initializer

Server: Server.c
	gcc -Wall -o Server Server.c

Client: Client.c
	gcc -Wall -o Client Client.c

main: main.c
	gcc -Wall -o Initializer main.c
	./Initializer
