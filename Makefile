CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I./include -I/opt/homebrew/opt/curl/include -I/opt/homebrew/opt/json-c/include
LDFLAGS = -L/opt/homebrew/opt/curl/lib -L/opt/homebrew/opt/json-c/lib -lcurl -ljson-c

SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
EXECUTABLE = $(BIN_DIR)/english

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

install: $(EXECUTABLE)
	mkdir -p $(DESTDIR)/usr/local/bin
	cp $(EXECUTABLE) $(DESTDIR)/usr/local/bin/

uninstall:
	rm -f $(DESTDIR)/usr/local/bin/english
