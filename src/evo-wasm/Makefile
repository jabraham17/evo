-include $(ROOT_PROJECT_DIRECTORY)options.mk
-include $(ROOT_PROJECT_DIRECTORY)src/dependencies.mk
LIBRARIES= $(evo-wasm)
TARGET=$(BIN_DIRECTORY)evo.js

-include $(ROOT_PROJECT_DIRECTORY)src/executable.mk

# add js depen
#  
JS_LIB=$(SRC_PATH)library.js
override LDFLAGS_FINAL+= --js-library $(JS_LIB) -sMODULARIZE=1 -sEXPORT_ES6=0 -sINVOKE_RUN=0 -Wl,--shared-memory -mbulk-memory -matomics -pthread -sEXPORT_NAME=Mod
$(TARGET): $(JS_LIB) install

.PHONY: install
install: $(SRC_PATH)evo-main.js $(SRC_PATH)index.html $(SRC_PATH)evo-worker.js
	$(AT)cp $^ $(BIN_DIRECTORY)
