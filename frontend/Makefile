WASM_BIN := dashboard.wasm
WASM_SOURCES := $(shell find -name '*.c')
WASM_CC ?= clang

CFLAGS += --target=wasm32 -Wall -Wextra -MMD -MP --no-standard-libraries -ffreestanding -Wl,--export-table,--no-entry,--allow-undefined,--export=start -O3

.PHONY:
all: $(WASM_BIN)

$(WASM_BIN): $(WASM_SOURCES)
	$(WASM_CC) $(CFLAGS) $^ -o $@

.PHONY:
clean:
	rm $(WASM_BIN) *.d
