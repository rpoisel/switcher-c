CC := gcc
RM := rm

BIN := switcher
DIR_SRC := src
DIR_INC := include
OBJ := $(patsubst %.c,%.o,$(wildcard $(DIR_SRC)/*.c))

CFLAGS := $(shell dpkg-buildflags --get CFLAGS) \
    -c -W -Wall -I.. -DNO_SSL -DNO_CGI -g -pipe \
    -I$(DIR_INC)
LDFLAGS := $(shell dpkg-buildflags --get LDFLAGS) \
    -pthread

all: $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJ)

.PHONY: clean

clean:
	-$(RM) $(BIN) $(OBJ)
