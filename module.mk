local_dir              := .
local_lib              := 
local_program          := $(addprefix $(DIR_BIN)/,switcher)
local_inc              := include mongoose
local_src              := $(addprefix $(DIR_SRC)/$(local_dir)/src/,\
    main.c \
    http.c \
    ini.c \
    io.c \
    io_i2c.c \
    parse_config.c \
    pcf8574.c \
    pfio.c \
    piface.c)
local_objs             := $(addprefix $(DIR_OBJ)/,$(subst .c,.o,$(local_src)))
local_dep              := $(addprefix $(DIR_OBJ)/,mongoose/libmongoose.a)
local_lib_dep          := -lpthread

libraries              += $(local_lib)
sources                += $(local_src)
includes               += $(local_inc)
objects                += $(local_objs)
programs               += $(local_program)

$(local_program): $(local_objs) $(local_dep)
	$(CC) $(LDFLAGS) -o $@ $^ $(local_lib_dep)
