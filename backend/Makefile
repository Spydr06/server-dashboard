SOURCES = $(shell find -name "*.c")
OBJECTS = $(patsubst %, $(BUILD_DIR)/%.o, $(SOURCES))

SERVER_BIN = $(BUILD_DIR)/server

LIBS = libsystemd

CFLAGS += -std=gnu99 -Wall -Wextra -g
LDFLAGS += -lpthread

CFLAGS += $(foreach lib, $(LIBS), $(shell pkg-config --cflags $(lib))) 
LDFLAGS += $(foreach lib, $(LIBS), $(shell pkg-config --libs $(lib)))

.PHONY:
all: $(SERVER_BIN)

$(SERVER_BIN): $(OBJECTS)
	$(CC) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/%.c.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -MF "$(@:%.o=%.d)" -c $< -o $@

