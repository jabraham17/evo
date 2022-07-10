
C_SOURCES=$(wildcard *.c) $(wildcard */*.c) $(wildcard */*/*.c)
C_OBJECTS=$(patsubst %.c,%.o,$(C_SOURCES))
A_SOURCES=$(wildcard *.asm) $(wildcard */*.asm) $(wildcard */*/*.asm)
A_OBJECTS=$(filter-out $(START),$(patsubst %.asm,%_a.o,$(A_SOURCES)))

YL_SOURCES=$(wildcard *.l) $(wildcard */*.l) $(wildcard */*/*.l) $(wildcard *.yy) $(wildcard */*.yy) $(wildcard */*/*.yy)
YL_OBJECTS=$(patsubst %,%.o,$(YL_SOURCES))

OBJECTS+=$(C_OBJECTS) $(A_OBJECTS) $(YL_OBJECTS)
DEPENDS=$(patsubst %.c,%.d,$(C_SOURCES))
DEPENDS+= $(patsubst %.asm,%_a.d,$(A_SOURCES))

REL_PATH=$(shell realpath --relative-to $(ROOT_PROJECT_DIRECTORY) .)/
SRC_PATH=$(ROOT_PROJECT_DIRECTORY)$(REL_PATH)
OBJ_PATH=$(OBJ_DIRECTORY)$(REL_PATH)
OBJECTS:=$(addprefix $(OBJ_PATH),$(OBJECTS))
DEPENDS:=$(addprefix $(OBJ_PATH),$(DEPENDS))

override INCLUDE+= -I$(ROOT_PROJECT_DIRECTORY)src
LIBRARIES_FILE_NAMES=$(patsubst %,$(LIB_DIRECTORY)lib%.a,$(LIBRARIES))
override LDLIBS+=$(patsubst %,-l%,$(LIBRARIES))

.DEFAULT_GOAL: all
.PHONY: all
all: $(OBJ_PATH) $(TARGET)
	@:

$(OBJ_PATH):
	$(AT)mkdir -p $@

.PHONY: cppcheck
cppcheck: $(C_SOURCES)
	$(AT)cppcheck $(INCLUDE) $^ -q

# not needed, toplevel handles deleting dir
# .PHONY: clean
# clean:
# 	$(AT)$(RM) $(TARGET) $(DEPENDS) $(OBJECTS)

$(OBJ_PATH)%.o: $(SRC_PATH)%.c Makefile
	$(CC) $(CFLAGS) -MMD -MP -MF $(patsubst $(SRC_PATH)%.c,$(OBJ_PATH)%.d,$<) $(INCLUDE) -c $< -o $@

# $(OBJ_PATH)%.E: $(SRC_PATH)%.c Makefile
# 	$(AT)mkdir -p $(dir $@)
# 	$(CC) $(CFLAGS) -E -MMD -MP -MF $(patsubst $(SRC_PATH)%.c,$(OBJ_PATH)%.d,$<) $(INCLUDE) -c $< -o $@

$(OBJ_PATH)%_a.o: $(SRC_PATH)%.asm Makefile
	$(AS) -felf $(CONSTANTS) $< -o $@ -MD $(patsubst $(SRC_PATH)%.asm,$(OBJ_PATH)%.d,$<)


_make_generated_c_source = $(patsubst $(SRC_PATH)%,$(OBJ_PATH)%.c,$1)

# using $(CFLAGS) causes errors, my guess is DXOPEN=700

$(OBJ_PATH)%.l.o: $(SRC_PATH)%.l $(OBJ_PATH)%.yy.o Makefile
	$(LEX) $(LFLAGS) -o $(call _make_generated_c_source,$<) $<
	$(CC)  -c -x c $(call _make_generated_c_source,$<) -x none -o $@ $(INCLUDE) -I$(SRC_PATH) -I$(OBJ_PATH)

$(OBJ_PATH)%.yy.o: $(SRC_PATH)%.yy Makefile
	$(YACC) $(YFLAGS) -o $(call _make_generated_c_source,$<) $< -d
	$(CC)  -c -x c $(call _make_generated_c_source,$<) -x none -o $@ $(INCLUDE) -I$(SRC_PATH)

-include $(DEPENDS)
