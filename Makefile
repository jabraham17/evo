
CHPL=chpl
CC=clang
SRC=src
BUILD=build
TARGET=evo

CHPL_SOURCES=$(wildcard $(SRC)/*.chpl) $(wildcard $(SRC)/*/*.chpl) $(wildcard $(SRC)/*/*/*.chpl)
DIRS=$(sort $(dir $(CHPL_SOURCES)))



CHPL_FLAGS_=
CHPL_FLAGS_+= $(CHPL_FLAGS)
CHPL_FLAGS_+= --permit-unhandled-module-errors
CHPL_FLAGS_+= -scPtrToStringBytesBufferAddress=true

ifeq ($(DEBUG),1)
CHPL_FLAGS_+= --target-compiler=gnu -g --preserve-inlined-line-numbers --savec $(BUILD)/$(TARGET).src.c
endif
ifeq ($(EMIT_LLVM),1)
CHPL_FLAGS_+= --target-compiler=llvm --savec $(BUILD)/$(TARGET).src.llvm
endif

.PHONY: all
all: $(BUILD)/$(TARGET) $(BUILD)/qoiconv

.PHONY: clean
clean:
	$(RM) $(BUILD) 

$(BUILD)/$(TARGET): $(CHPL_SOURCES)
	@mkdir -p $(BUILD)
	$(CHPL) $(SRC)/$(TARGET)/main.chpl $(CHPL_FLAGS_) $(foreach d,$(DIRS),--module-dir $(d)) -o $@



$(BUILD)/qoiconv: $(SRC)/qoiconv/qoiconv.c
	$(CC) $^ -o $@ -O3
