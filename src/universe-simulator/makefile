COMPILER = gcc
CFLAGS = -Wall -Wextra -g -Werror -pedantic
CONFIGFLAGS = -lconfig
SDLFLAGS = -lSDL2 -lSDL2_image -lSDL2_gfx -lSDL2_ttf
ZMQFLAGS = -lzmq
PROTOCFLAGS = -lprotobuf-c

TARGET = universe-simulator
SRCS = universe-simulator.c universe-data.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(COMPILER) $(CFLAGS) -o $(TARGET) $(SRCS) $(CONFIGFLAGS) $(SDLFLAGS) $(ZMQFLAGS) $(PROTOCFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
