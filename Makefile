AR?=ar
CC?=gcc
CFLAGS?=-Wall -O2
LDFLAGS?=-lm

.PHONY: all tools

all: libmidi.a
tools: mididump

libmidi.a: buffer.o stream.o midi.o midi_file.o midi_reader.o midi_track.o
	$(AR) cr $@ $^

mididump: mididump.o libmidi.a
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -c $< -o $@

-include $(OBJS:.o=.d)

clean:
	rm -f *.o *.a *.d mididump
