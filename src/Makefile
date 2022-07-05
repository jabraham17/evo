TOPTARGETS=all clean

SUBDIRS= dot img common simulator viz evo viewbrain

.PHONY: $(TOPTARGETS)
$(TOPTARGETS): $(SUBDIRS)

# dependencies
evo: img simulator viz common

viewbrain: img simulator viz dot

simulator: common

img: common

dot: common

viz: simulator img dot common

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@echo "MAKE -C $@ $(MAKECMDGOALS)"
	@$(MAKE) --no-print-directory -C $@ $(MAKECMDGOALS)
