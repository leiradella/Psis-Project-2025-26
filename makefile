COMPILER = gcc
CFLAGS = -Wall -Wextra -O2 -Werror -pedantic
CONFIGFLAGS = -lconfig++ 
SDLFLAGS = -lSDL2 -lSDL2_image -lSDL2_gfx -lSDL2_ttf
ZMQFLAGS = -lzmq
PROTOCFLAGS = -lprotobuf-c


universe-simulator: src/universe-simulator/universe-simulator.c src/universe-simulator/universe-data.c
	$(COMPILER) $(CFLAGS) -o universe-simulator $^ $(CONFIGFLAGS) $(SDLFLAGS) $(ZMQFLAGS) $(PROTOCFLAGS)