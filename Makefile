RM := rm
STRIP := strip

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
	$(CC_PREFIX)$(STRIP) -s $(BIN)

debug: $(BIN)

%.o: %.c
	$(CC_PREFIX)$(CC) $(CFLAGS) -o $@ $<

$(BIN): $(OBJ)
	$(CC_PREFIX)$(CC) $(LDFLAGS) -o $(BIN) $(OBJ)

.PHONY: clean all debug release

clean:
	-$(RM) $(BIN) $(OBJ)
