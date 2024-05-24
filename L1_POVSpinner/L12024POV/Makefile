# Project Name
PROJECT = L12024POV

# AVR MCU
MCU = attiny4313

# Source Files
SRC = main.c

# Include Directories
INCLUDES = -Iimages

# Compiler
CC = avr-gcc

# Compiler Flags
CFLAGS = -mmcu=$(MCU) -Os $(INCLUDES) -MMD

# Linker Flags
LDFLAGS = -lm

# Output files
ELF = $(PROJECT).elf
HEX = $(PROJECT).hex

# Dependency files
DEP = $(SRC:.c=.d)

# Default target
all: $(HEX)

# Compile and link
$(ELF): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Convert ELF to HEX
$(HEX): $(ELF)
	avr-objcopy -O ihex $< $@

# Clean up
clean:
	rm -f $(ELF) $(HEX) $(DEP)

# Include the dependency files
-include $(DEP)

# Makefile help
help:
	@echo "Usage:"
	@echo "  make         # Build the project"
	@echo "  make clean   # Remove build files"
	@echo "  make help    # Display this help"

.PHONY: all clean help
