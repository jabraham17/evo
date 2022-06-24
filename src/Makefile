TOPTARGETS=all clean

SUBDIRS= dot img main common simulator viz

.PHONY: $(TOPTARGETS)
$(TOPTARGETS): $(SUBDIRS)

# dependencies
main: img dot simulator viz

simulator: common

img: common

dot: common

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@echo "MAKE -C $@ $(MAKECMDGOALS)"
	@$(MAKE) --no-print-directory -C $@ $(MAKECMDGOALS)
