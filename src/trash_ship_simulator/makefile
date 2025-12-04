all: universe-client universe-server

universe-client: universe-client.c msg.pb-c.h
	gcc -g \
		universe-client.c -o universe-client \
		msg.pb-c.c \
		-lSDL2 -lSDL2_image -lzmq -lprotobuf-c

universe-server: universe-server.c msg.pb-c.h
	gcc -g universe-server.c -o universe-server \
		msg.pb-c.c universe-data.c physics-rules.c display.c \
		-lSDL2 -lSDL2_image -lzmq -lprotobuf-c  -lSDL2_ttf -lconfig -lSDL2_gfx

clean:
	rm -f universe-client universe-server