TOPTARGETS=all clean

SUBDIRS= img main simulator viz common

.PHONY: $(TOPTARGETS)
$(TOPTARGETS): $(SUBDIRS)

# dependencies
main: img simulator viz

simulator: common

img: common

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@echo "MAKE -C $@ $(MAKECMDGOALS)"
	@$(MAKE) --no-print-directory -C $@ $(MAKECMDGOALS)
