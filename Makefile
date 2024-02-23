BUILD_DIR = "./build"

.PHONY:
all: backend frontend

$(BUILD_DIR):
	mkdir -p $@

.PHONY:
backend: $(BUILD_DIR)
	$(MAKE) -C backend all BUILD_DIR=$(shell realpath $(BUILD_DIR))

.PHONY: 
frontend: $(BUILD_DIR)
	$(MAKE) -C frontend all BUILD_DIR=$(shell realpath $(BUILD_DIR))

.PHONY:
clean:
	rm -rf $(BUILD_DIR)
	$(MAKE) -C frontend clean
