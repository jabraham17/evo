-include $(ROOT_PROJECT_DIRECTORY)options.mk

TOPTARGETS=all clean cppcheck
SUBDIRS= simulator common img viz dot

ifeq ($(WASM),0)
# native targets
SUBDIRS+= evo dumpbrain zpipe
else
# wasm targets
SUBDIRS+= evo-wasm
endif

# ifeq ($(THREADED),1)
# SUBDIRS+= pthreadpool
# endif

.PHONY: $(TOPTARGETS)
$(TOPTARGETS): $(SUBDIRS)

-include $(ROOT_PROJECT_DIRECTORY)src/dependencies.mk
$(call make_prereqs)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@echo "MAKE -C src/$@ $(MAKECMDGOALS)"
	@$(MAKE) --no-print-directory -C $@ $(MAKECMDGOALS)
