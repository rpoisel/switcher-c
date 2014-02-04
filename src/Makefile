CC := gcc
CFLAGS := $(shell dpkg-buildflags --get CFLAGS) \
    -W -Wall -I.. -DNO_SSL -DNO_CGI -g -pipe
LDFLAGS := $(shell dpkg-buildflags --get LDFLAGS) \
    -pthread
RM := rm

BIN := switcher
OBJ := $(patsubst %.c,%.o,$(wildcard *.c))


all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJ)

.PHONY: clean

clean:
	-$(RM) $(BIN) $(OBJ)
