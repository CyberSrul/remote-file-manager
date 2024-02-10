compiler = gcc
executables = client server
flags = -Wall -g3


all: client server

client:	client.c protocol.h
		$(compiler) $(flags) client.c -o client

server:	server.c protocol.h
		$(compiler) $(flags) server.c -o server

clean:
		rm -r $(executables)