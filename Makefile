EXEC = mips-simulator
BIN_DIR = $(PWD)/bin
BUILD_DIR = $(PWD)/build
SRC_DIRS = src

SRCS = $(shell find $(SRC_DIRS) -name *.c)
OBJS = $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

INC_DIRS = include
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CFLAGS = $(INC_FLAGS) -Wall -MMD -MP
LDFLAGS =

CC = gcc

all: $(BIN_DIR)/$(EXEC) $(BIN_DIR)/hello.text.bin $(BIN_DIR)/hello.data.bin

$(BIN_DIR)/$(EXEC): $(OBJS)
	mkdir -p bin
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/hello.o: hello.s
	mips-linux-gnu-as hello.s -o $(BUILD_DIR)/hello.o

$(BUILD_DIR)/hello: $(BUILD_DIR)/hello.o
	mips-linux-gnu-ld $(BUILD_DIR)/hello.o -o $(BUILD_DIR)/hello

$(BIN_DIR)/hello.text.bin: $(BUILD_DIR)/hello
	mips-linux-gnu-objcopy -O binary --only-section=.text $(BUILD_DIR)/hello $(BIN_DIR)/hello.text.bin

$(BIN_DIR)/hello.data.bin: $(BUILD_DIR)/hello
	mips-linux-gnu-objcopy -O binary --only-section=.text $(BUILD_DIR)/hello $(BIN_DIR)/hello.data.bin


.PHONY: run format clean

run: all
	$(BIN_DIR)/$(EXEC)

format:
	find src/* -exec clang-format -i {} \;

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)/$(EXEC) $(BIN_DIR)/*.bin

-include $(DEPS)
