all: universe-client universe-server

universe-client: universe-client.c msg.pb-c.h
	gcc -g universe-client.c msg.pb-c.c -o universe-client \
		-lSDL2 -lSDL2_image -lzmq -lprotobuf-c

universe-server: universe-server.c msg.pb-c.h
	gcc -g universe-server.c msg.pb-c.c -o universe-server \
		-lSDL2 -lSDL2_image -lzmq -lprotobuf-c

clean:
	rm -f universe-client universe-server