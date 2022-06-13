
TOPTARGETS=all

SUBDIRS=src

MKFILE_PATH=$(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIR=$(dir $(MKFILE_PATH))
export ROOT_PROJECT_DIRECTORY=$(MKFILE_DIR)

BUILD=build
export BUILD_DIRECTORY=$(ROOT_PROJECT_DIRECTORY)$(BUILD)/

export BIN_DIRECTORY=$(BUILD_DIRECTORY)bin/
export OBJ_DIRECTORY=$(BUILD_DIRECTORY)obj/
export LIB_DIRECTORY=$(BUILD_DIRECTORY)lib/

export OS=$(shell uname)
ifeq ($(OS),Linux)
export CC=/usr/bin/gcc
export LD:=$(CC)
export YACC=/usr/bin/bison
export LEX=/usr/bin/flex
export AR=/usr/bin/ar
export RANLIB=/usr/bin/ranlib
else ifeq ($(OS),Darwin)
export CC=/usr/local/opt/llvm/bin/clang
export LD:=$(CC)
export YACC=/usr/local/opt/bison/bin/bison
export LEX=/usr/local/opt/flex/bin/flex
export AR=/usr/local/opt/llvm/bin/llvm-ar
export RANLIB=/usr/local/opt/llvm/bin/llvm-ranlib
else
$(error Unsupported build on $(OS))
endif
export AS=nasm


export VERBOSE=0
ifeq ($(VERBOSE),0)
export AT=@
else
export AT=
endif

# define make_rel_to_root
# $(shell realpath --relative-to $(ROOT_PROJECT_DIRECTORY) $1)
# endef

define _generate_verbose_call
$1_0 = @printf "%s %s\n" $1 $$$$(shell sed s%$(ROOT_PROJECT_DIRECTORY)%% <<< $$$$@); $($1)
$1_1 = $($1)
export $1 = $$($1_$(VERBOSE))
endef
define generate_verbose_call
$(eval $(call _generate_verbose_call,$1))
endef

map = $(foreach a,$(2),$(call $(1),$(a)))

$(call map,generate_verbose_call,CC LD YACC LEX AR RANLIB)

# clang cant find gcc_s, gcc doesnt compile things correctly
# clang with rtlib=compiler-rt has problems with relocation

ifeq ($(OS),Linux)
override LDFLAGS+= 
#-fuse-ld=/usr/bin/gold
endif

override CFLAGS+= -DOS_SPECIFIC=$(OS) -Wno-comment

export EXTENSION=c

ifeq ($(DEBUG),1)
override CFLAGS+= -DDEBUG=1 -g -O0 
# -fno-pie
override CFLAGS+= -DROOT_PROJECT_DIRECTORY="\"$(ROOT_PROJECT_DIRECTORY)\""
else
override CFLAGS+= -O3
endif

override CFLAGS+= -Wall -Wextra
override CFLAGS+= -masm=intel
override CFLAGS+= -std=c11
override CFLAGS+= -D_XOPEN_SOURCE=700
override ASFLAGS+=
override LDFLAGS+=
override LDFLAGS_FINAL+=
override INCLUDE+=
override YFLAGS+= -Wall
override LFLAGS+=

ifeq ($(OS),Darwin)
override LDFLAGS+= -L/usr/local/opt/llvm/lib
override LDFLAGS_FINAL+= -Wl,-rpath,/usr/local/opt/llvm/lib
override INCLUDE+= -I/usr/local/opt/llvm/include
endif


export CFLAGS
export ASFLAGS
export LDFLAGS
export LDFLAGS_FINAL
export INCLUDE
export YFLAGS
export LFLAGS

.PHONY: $(TOPTARGETS)

all: $(BUILD_DIRECTORY) $(BIN_DIRECTORY) $(LIB_DIRECTORY) $(OBJ_DIRECTORY)
	@:

$(BUILD_DIRECTORY) $(BIN_DIRECTORY) $(LIB_DIRECTORY) $(OBJ_DIRECTORY):
	$(AT)mkdir -p $@

clean:
	$(RM) -r $(BUILD_DIRECTORY)

$(TOPTARGETS): $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@echo "MAKE -C $@ $(MAKECMDGOALS)"
	@$(MAKE) --no-print-directory -C $@ $(MAKECMDGOALS)


# .PHONY: scan_build
# scan_build:
# 	/usr/local/opt/llvm/bin/scan-build

# define echo_var
# $(shell echo "$1=$($1_1)")
# endef
# $(call map,echo_var,CC LD YACC LEX AR RANLIB)
.PHONY: dump_paths
dump_paths:
	@echo "CC=$(CC_1)" 
	@echo "LD=$(LD_1)" 
	@echo "YACC=$(YACC_1)" 
	@echo "LEX=$(LEX_1)" 
	@echo "AR=$(AR_1)" 
	@echo "RANLIB=$(RANLIB_1)" 
