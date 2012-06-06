#AVR-GCC Makefile
PROJECT = planefinder
SOURCES = src/main.c
CC = avr-gcc
OBJCOPY = avr-objcopy
MMCU = attiny13

DUMMY_BINARY:=$(shell mkdir -p binary)

CFLAGS = -mmcu=$(MMCU) -Wall -Os -std=gnu99

binary/$(PROJECT).hex: binary/$(PROJECT).out
	$(OBJCOPY) -j .text -O ihex binary/$(PROJECT).out binary/$(PROJECT).hex

binary/$(PROJECT).out: $(SOURCES)
	$(CC) $(CFLAGS) -I./ -o binary/$(PROJECT).out $(SOURCES)

program: binary/$(PROJECT).hex
	avrdude -p t13 -c usbasp -e -U flash:w:binary/$(PROJECT).hex

clean:
	rm -f binary/$(PROJECT).out
	rm -f binary/$(PROJECT).hex
