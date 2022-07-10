
MKFILE_PATH=$(abspath $(lastword $(MAKEFILE_LIST)))
MKFILE_DIR=$(dir $(MKFILE_PATH))
export ROOT_PROJECT_DIRECTORY=$(MKFILE_DIR)

TOPTARGETS=all
SUBDIRS=src
.PHONY: $(TOPTARGETS)

-include options.mk

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

.PHONY: dump_paths
dump_paths:
	$(info CC=$(CC_1))
	$(info LD=$(LD_1))
	$(info YACC=$(YACC_1))
	$(info LEX=$(LEX_1))
	$(info AR=$(AR_1))
	$(info RANLIB=$(RANLIB_1))

