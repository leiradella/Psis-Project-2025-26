all: universe-client universe-server

universe-client: universe-client.c
	gcc -g universe-client.c -o universe-client \
		-lSDL2 -lSDL2_image -lzmq

universe-server: universe-server.c
	gcc -g universe-client.c -o universe-server \
		-lSDL2 -lSDL2_image

clean:
	rm -f universe-client