compiler := gcc
executables = client server


all: client server

client:	client.c protocol.h
		$(compiler) client.c -o client

server:	server.c protocol.h
		$(compiler) server.c -o server

clean:
		rm -r $(executables)