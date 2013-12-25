CC := gcc
CFLAGS := -W -Wall -I.. -DNO_SSL -DNO_CGI -g -pipe
LDFLAGS := -pthread -ldl
RM := rm

BIN := switcher
OBJ := \
    	main.o \
	mongoose.o \
	ini.o \
	parse_i2c.o \
	http.o \
	i2c.o \
	i2c_io.o \
	pcf8574.o \


all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(LDFLAGS) -o $(BIN) $(OBJ)

.PHONY: clean

clean:
	-$(RM) $(BIN) $(OBJ)
