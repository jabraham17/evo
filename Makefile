
CHPL=chpl
SRC=src
BUILD=build
TARGET=evo

DIRS=dot img simulator
CHPL_SOURCES=$(wildcard $(SRC)/*.chpl) $(wildcard $(SRC)/*/*.chpl) $(wildcard $(SRC)/*/*/*.chpl)


CHPL_FLAGS_=
CHPL_FLAGS_+= $(CHPL_FLAGS)
CHPL_FLAGS_+= --permit-unhandled-module-errors

$(BUILD)/$(TARGET): $(CHPL_SOURCES)
	@mkdir -p $(BUILD)
	$(CHPL) $(SRC)/$(TARGET)/main.chpl $(CHPL_FLAGS_) $(foreach d,$(DIRS),--module-dir $(SRC)/$(d)) -o $@ 



