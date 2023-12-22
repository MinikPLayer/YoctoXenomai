SKIN=alchemy
MAIN_SRC=jitter_multi
TARGET=jitter_multi

LM=-lm 


CFLAGS := $(shell xeno-config --skin=alchemy --cflags)
LDFLAGS := $(LM) $(shell xeno-config --skin=alchemy --ldflags)
CC := gcc

$(TARGET): $(MAIN_SRC).c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)