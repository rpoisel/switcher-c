RM := rm

BIN := switcher
DIR_SRC := src
DIR_INC := include
OBJ := $(patsubst %.c,%.o,$(wildcard $(DIR_SRC)/*.c))

CFLAGS := $(shell dpkg-buildflags --get CFLAGS) \
    -c -W -Wall -I.. -DNO_SSL -DNO_CGI -pipe \
    -I$(DIR_INC)
LDFLAGS := $(shell dpkg-buildflags --get LDFLAGS) \
    -pthread

all: CFLAGS += -O2
all: $(BIN)

debug: CFLAGS += -g -O0
debug: $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJ)

.PHONY: clean all debug

clean:
	-$(RM) $(BIN) $(OBJ)
