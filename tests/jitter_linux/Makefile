SKIN=alchemy
MAIN_SRC=jitter_linux
TARGET=jitter_linux

LM=-lm 


CFLAGS := $(shell xeno-config --skin=alchemy --cflags)
LDFLAGS := $(LM) $(shell xeno-config --skin=alchemy --ldflags)
CC := g++

$(TARGET): $(MAIN_SRC).cpp
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)