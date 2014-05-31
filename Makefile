# output directories
DIR_OUTPUT := output
DIR_PLATFORM ?= native
DIR_TARGET ?= debug

# directory structure
DIR_TOP := $(dir $(lastword $(MAKEFILE_LIST)))
DIR_OBJ = $(DIR_OUTPUT)/$(DIR_PLATFORM)/$(DIR_TARGET)/obj
DIR_BIN = $(DIR_OUTPUT)/$(DIR_PLATFORM)/$(DIR_TARGET)/bin
DIR_MODULE := .
DIR_SRC := $(DIR_TOP)$(DIR_MODULE)/src
DIR_INC := $(DIR_TOP)$(DIR_MODULE)/include

# file extensions
EXT_SRC := .c

# derived paths and files
BIN = $(DIR_BIN)/switcher
OBJ = \
    $(addprefix $(DIR_OBJ)/,$(patsubst %$(EXT_SRC),%$(EXT_OBJ),$(notdir $(wildcard $(DIR_SRC)/*$(EXT_SRC)))))

# programs
RM := rm
STRIP := strip
CC := gcc
MKDIR := mkdir
TOUCH := touch

# compiler/linker flags
# $(shell dpkg-buildflags --get CFLAGS)
# $(shell dpkg-buildflags --get LDFLAGS)
CFLAGS := -c -W -Wall -DNO_SSL -DNO_CGI -pipe \
    -I$(DIR_INC)
LDFLAGS := -pthread

ifeq ($(BUILD_TARGET),release)
	DIR_TARGET := release
	CFLAGS += -O2
	EXT_OBJ = _r.o
else
	DIR_TARGET := debug
	CFLAGS += -O0 -g -DDEBUG
	EXT_OBJ = _d.o
endif

.PHONY: clean all 

all: $(BIN)

$(OBJ): $(DIR_OBJ)/%$(EXT_OBJ): \
    $(DIR_SRC)/%$(EXT_SRC) \
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
	-$(RM) -rf $(DIR_OUTPUT)/$(DIR_PLATFORM)/$(DIR_TARGET)
ifeq ($(wildcard $(DIR_OUTPUT)/$(DIR_PLATFORM)/*),)
	-$(RM) -rf $(DIR_OUTPUT)/$(DIR_PLATFORM)
endif
ifeq ($(wildcard $(DIR_OUTPUT)/*),)
	-$(RM) -rf $(DIR_OUTPUT)
endif
