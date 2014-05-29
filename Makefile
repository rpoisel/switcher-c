RM := rm
STRIP := strip
CC := gcc

BIN := switcher
DIR_SRC := src
DIR_INC := include
OBJ := $(patsubst %.c,%.o,$(wildcard $(DIR_SRC)/*.c))

CFLAGS := $(shell dpkg-buildflags --get CFLAGS) \
    -c -W -Wall -I.. -DNO_SSL -DNO_CGI -pipe \
    -I$(DIR_INC)
LDFLAGS := $(shell dpkg-buildflags --get LDFLAGS) \
    -pthread

all: release

release: debug
	$(CROSS_COMPILE)$(STRIP) -s $(BIN)

debug: $(BIN)

%.o: %.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $<

$(BIN): $(OBJ)
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -o $(BIN) $(OBJ)

.PHONY: clean all debug release

clean:
	-$(RM) $(BIN) $(OBJ)
