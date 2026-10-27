# Compiler
CC      ?= cc
CFLAGS  ?= -std=c17 -Wall -Wextra -Wpedantic -O2
LDFLAGS ?=
LDLIBS  ?=

# Platform detection
OS      ?= $(shell uname -s)
ARCH    ?= $(shell uname -m)

# Directories
SRC_DIR   := src
BUILD_DIR := target/$(OS)-$(ARCH)

PREFIX ?= /usr/local
BINDIR  := $(PREFIX)/bin
DESTDIR ?=

# Project
TARGET  := $(BUILD_DIR)/chex
SOURCES := $(SRC_DIR)/main.c $(SRC_DIR)/dump.c
HEADERS := $(SRC_DIR)/dump.h

.PHONY: all build clean install platform

all: build

# Build
build: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SOURCES) $(LDFLAGS) $(LDLIBS) -o $@

# Platform information
platform:
	@echo "OS:   $(OS)"
	@echo "ARCH: $(ARCH)"
	@echo "CC:   $(CC)"

# Clean
clean:
	rm -rf target

# Install
install: build
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 $(TARGET) $(DESTDIR)$(BINDIR)/chex
