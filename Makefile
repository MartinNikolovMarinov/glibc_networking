CC            = g++
COMMON_CFLAGS = -ggdb -std=c++17 -Wno-unknown-pragmas
CFLAGS        = $(COMMON_CFLAGS) -O0 -g -Wall -Wno-unused
DEBUG_DEFINES = -DDEBUG=1
SRC           = main.cpp $(shell find ./src/ -name "*.cpp")
CORE_LIB_SRC  = $(shell find ./libs/core/src -name "*.cpp")
OUT_DIR       = build
BIN_NAME      = main

# HELP TARGET:

.PHONY: help
help:
	@echo Supported targets:
	@cat $(MAKEFILE_LIST) | grep -e "^[\.a-zA-Z0-9_-]*: *.*## *" | \
		awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-35s\033[0m %s\n", $$1, $$2}'

.DEFAULT_GOAL := help

# MAIN TARGETS:

.PHONY: build
build: ## Build the project in the build folder. Creates ./build folder if it does not exist.
	mkdir -p $(OUT_DIR)
	$(CC) $(CFLAGS) $(DEBUG_DEFINES) -o $(OUT_DIR)/$(BIN_NAME) $(SRC) $(CORE_LIB_SRC)

.PHONY: run
run: build ## build and run post build.
	(OUT_DIR=${OUT_DIR} scripts/quick_run.sh)

.PHONY: generate_asm
generate_asm: ## Uses objdump -S to generate asm from the binary. Expects the output binaries to be generated first by other make targets.
	objdump -M intel -S $(OUT_DIR)/$(BIN_NAME) > $(OUT_DIR)/$(BIN_NAME).S

.PHONY: clean
clean: ## Deletes the build folder.
	rm -rf $(OUT_DIR)

# UTIL TARGETS:

.PHONY: post_debug_run
post_debug_run: ## specifically implemented because VS Code can't be botherd to run shell scripts on launch.
	sudo setcap cap_net_admin=eip ${OUT_DIR}/main
	sudo ip addr add 192.168.0.1/24 dev tun0
	sudo ip link set up dev tun0

.PHONY: ping
ping: ## test ping the tun0 interface on some IP address on 192.168.0.2
	ping -I tun0 192.168.0.2

.PHONY: sniff
sniff: ## sniff traffic to tun0 with tshark.
	sudo tshark -i tun0

.PHONY: del_tun_interface
del_tun_interface: ## delete the created tun0 interface. In case the tun device was not destroyed properly.
	sudo ip link delete tun0