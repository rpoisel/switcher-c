ifeq ($(BUILD_TARGET),release)
	DIR_TARGET := release
	EXT_OBJ = _r.o
else
	DIR_TARGET := debug
	EXT_OBJ = _d.o
endif

# output directories
DIR_OUTPUT := output
PLATFORM ?= native

# directory structure
DIR_TOP := $(dir $(lastword $(MAKEFILE_LIST)))
DIR_SRC := $(DIR_TOP)
DIR_OBJ = $(DIR_OUTPUT)/$(PLATFORM)/$(DIR_TARGET)/obj
DIR_BIN = $(DIR_OUTPUT)/$(PLATFORM)/$(DIR_TARGET)

modules        := mongoose .
programs       :=
sources        :=
includes       :=
objects        :=
libraries      :=
extra_clean    :=

object_dirs_deps = $(addsuffix /.stamp,$(sort $(dir $(objects))))

# programs
RM    := rm -rf
STRIP := $(CROSS_COMPILE)strip -s
CC    := $(CROSS_COMPILE)gcc
MKDIR := mkdir
TOUCH := touch
TEST  := test
GREP  := grep
PR    := pr
AWK   := awk
SORT  := sort

# user defined functions
define delete-empty-dir
    $(TEST) -d $1 -a "$$(ls -A $1 2>/dev/null)" || $(RM) $1
endef

all:

include $(addsuffix /module.mk,$(modules))

.PHONY: all
all: $(programs)
ifeq ($(BUILD_TARGET),release)
	$(STRIP) $(programs)
endif

.PHONY: libraries
libraries: $(libraries)

.PHONY: clean
clean:
ifneq ($(strip $(programs)),)
	-$(RM) $(programs)
endif
ifneq ($(strip $(libraries)),)
	-$(RM) $(libraries)
endif
ifneq ($(strip $(objects)),)
	-$(RM) $(objects)
endif
	-$(RM) $(DIR_OUTPUT)/$(PLATFORM)/$(DIR_TARGET)
	-$(call delete-empty-dir,$(DIR_OUTPUT)/$(PLATFORM))
	-$(call delete-empty-dir,$(DIR_OUTPUT))

# file extensions
EXT_SRC := .c

# compiler/linker flags
# $(shell dpkg-buildflags --get CFLAGS)
# $(shell dpkg-buildflags --get LDFLAGS)
CFLAGS := -c -W -Wall -DNO_SSL -DNO_CGI -pipe \
    $(addprefix -I,$(sort $(includes)))
ifeq ($(BUILD_TARGET),release)
	CFLAGS += -O2
else
	CFLAGS += -O0 -g -DDEBUG
endif
CPPFLAGS += -MMD -MP

all: $(programs)

.PHONY: help
help:
	@$(MAKE) --print-data-base --question | \
	$(GREP) -v -e '^no-such-target' -e '^makefile' |       \
	$(AWK) '/^[^.%][-A-Za-z0-9_]*:/                        \
                   { print substr($$1, 1, length($$1)-1) }' |  \
	$(SORT) |                                              \
	$(PR) --omit-pagination --width=80 --columns=4         \

$(DIR_OBJ)/%.o: \
    %.c \
    $(object_dirs_deps)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.stamp:
	$(MKDIR) -p $(dir $@)
	$(TOUCH) $@

-include $(objects:%.o=%.d)
