# output directories
DIR_OUTPUT := output
DIR_PLATFORM ?= native

# directory structure
DIR_TOP := $(dir $(lastword $(MAKEFILE_LIST)))
DIR_OBJ := $(DIR_OUTPUT)/$(DIR_PLATFORM)/obj
DIR_BIN := $(DIR_OUTPUT)/$(DIR_PLATFORM)/bin
DIR_MODULE := .
DIR_SRC := $(DIR_TOP)/$(DIR_MODULE)/src
DIR_INC := $(DIR_TOP)/$(DIR_MODULE)/include

# derived paths and files
BIN := $(DIR_BIN)/switcher
OBJ := \
    $(addprefix $(DIR_OBJ)/,$(patsubst %.c,%.o,$(notdir $(wildcard $(DIR_SRC)/*.c))))

# programs
RM := rm
STRIP := strip
CC := gcc
MKDIR := mkdir
TOUCH := touch

# compiler flags
CFLAGS := $(shell dpkg-buildflags --get CFLAGS) \
    -c -W -Wall -DNO_SSL -DNO_CGI -pipe \
    -I$(DIR_INC)
LDFLAGS := $(shell dpkg-buildflags --get LDFLAGS) \
    -pthread

.PHONY: clean all debug release

all: release

release: debug
	$(CROSS_COMPILE)$(STRIP) -s $(BIN)

debug: $(BIN)

$(OBJ): $(DIR_OBJ)/%.o: \
    $(DIR_SRC)/%.c \
    $(DIR_OBJ)/.d
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o $@ $<

$(BIN): \
    $(OBJ) \
    $(DIR_BIN)/.d
	$(CROSS_COMPILE)$(CC) $(LDFLAGS) -o $(BIN) $(OBJ)
	
%.d:
	$(MKDIR) -p $(dir $@)
	$(TOUCH) $@

clean:
	-$(RM) -rf $(BIN)
	-$(RM) -rf $(DIR_OUTPUT)/$(DIR_PLATFORM)
