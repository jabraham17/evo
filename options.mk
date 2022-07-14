
BUILD=build
BUILD_DIRECTORY=$(ROOT_PROJECT_DIRECTORY)$(BUILD)/

BIN_DIRECTORY=$(BUILD_DIRECTORY)bin/
OBJ_DIRECTORY=$(BUILD_DIRECTORY)obj/
LIB_DIRECTORY=$(BUILD_DIRECTORY)lib/

# erase automatic vars
.SUFFIXES:

OS=$(shell uname)
ifeq ($(OS),Linux)
CC=/usr/bin/clang
LD:=$(CC)
YACC=/usr/bin/bison
LEX=/usr/bin/flex
AR=/usr/bin/ar
RANLIB=/usr/bin/ranlib
else ifeq ($(OS),Darwin)
CC=/usr/local/opt/llvm/bin/clang
LD:=$(CC)
YACC=/usr/local/opt/bison/bin/bison
LEX=/usr/local/opt/flex/bin/flex
AR=/usr/local/opt/llvm/bin/llvm-ar
RANLIB=/usr/local/opt/llvm/bin/llvm-ranlib
else
$(error Unsupported build on $(OS))
endif
AS=nasm


VERBOSE=0
ifeq ($(VERBOSE),0)
AT=@
else
AT=
endif

define strip_root
$$(shell sed s%$(ROOT_PROJECT_DIRECTORY)%% <<< $1)
endef
define _generate_verbose_call
override $1_0=@printf "%s %s\n" $1 $(call strip_root,$$@); $($1)
override $1_1=$($1)
override $1=$$($1_$(VERBOSE))
endef
define generate_verbose_call
$(eval $(call _generate_verbose_call,$1))
endef

map = $(foreach a,$(2),$(call $(1),$(a)))
$(call map,generate_verbose_call,CC LD YACC LEX AR RANLIB)

override CFLAGS+= -DOS_SPECIFIC=$(OS) -Wno-comment

ifeq ($(DEBUG),1)
override CFLAGS+= -DDEBUG=1 -g -O0 
override LDFLAGS+= -g
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

# -L/usr/local/opt/zlib/lib
# -I/usr/local/opt/zlib/include
ifeq ($(OS),Darwin)
override LDFLAGS+= -L/usr/local/opt/llvm/lib -L/usr/local/lib -lpopt -lz
override LDFLAGS_FINAL+= -Wl,-rpath,/usr/local/opt/llvm/lib
override INCLUDE+= -I/usr/local/opt/llvm/include -I/usr/local/include
endif
ifeq ($(OS),Linux)
override LDFLAGS+= -lpopt -lz -lm
override INCLUDE+= -I/usr/include
endif

THREADED=0
ifeq ($(THREADED),1)
override CFLAGS+= -DTHREADED=1
endif
