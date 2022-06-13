TOPTARGETS=all clean

SUBDIRS= img main simulator viz

.PHONY: $(TOPTARGETS)
$(TOPTARGETS): $(SUBDIRS)

# dependencies
main: img simulator viz


.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@echo "MAKE -C $@ $(MAKECMDGOALS)"
	@$(MAKE) --no-print-directory -C $@ $(MAKECMDGOALS)
