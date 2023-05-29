
CHPL=chpl
SRC=src
BUILD=build
TARGET=evo

CHPL_SOURCES=$(wildcard $(SRC)/*.chpl) $(wildcard $(SRC)/*/*.chpl) $(wildcard $(SRC)/*/*/*.chpl)
DIRS=$(sort $(dir $(CHPL_SOURCES)))



CHPL_FLAGS_=
CHPL_FLAGS_+= $(CHPL_FLAGS)
CHPL_FLAGS_+= --permit-unhandled-module-errors

ifeq ($(DEBUG),1)
CHPL_FLAGS_+= --target-compiler=gnu -g --preserve-inlined-line-numbers --savec $(BUILD)/$(TARGET).src.c
endif
ifeq ($(EMIT_LLVM),1)
CHPL_FLAGS_+= --target-compiler=llvm --savec $(BUILD)/$(TARGET).src.llvm
endif

$(BUILD)/$(TARGET): $(CHPL_SOURCES)
	@mkdir -p $(BUILD)
	$(CHPL) $(SRC)/$(TARGET)/main.chpl $(CHPL_FLAGS_) $(foreach d,$(DIRS),--module-dir $(d)) -o $@ 



