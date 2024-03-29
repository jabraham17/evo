
evo=simulator common img viz dot
dumpbrain=simulator common img viz dot
zpipe=common
evo-wasm=simulator common img viz dot

# ifeq ($(THREADED),1)
# evo+= pthreadpool
# dumpbrain+= pthreadpool
# override LDLIBS+= -lpthread
# endif

define make_depen
$(eval $1: $($1))
endef
map = $(foreach a,$(2),$(call $(1),$(a)))
define make_prereqs
$(call map,make_depen,evo dumpbrain zpipe evo-wasm)
endef
