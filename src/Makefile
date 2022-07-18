-include $(ROOT_PROJECT_DIRECTORY)options.mk

TOPTARGETS=all clean cppcheck
SUBDIRS= simulator common img viz dot evo dumpbrain zpipe
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
