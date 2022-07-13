
evo=simulator common img viz dot
dumpbrain=simulator common img viz dot
zpipe=common

define make_depen
$(eval $1: $($1))
endef
map = $(foreach a,$(2),$(call $(1),$(a)))
define make_prereqs
$(call map,make_depen,evo dumpbrain zpipe)
endef
