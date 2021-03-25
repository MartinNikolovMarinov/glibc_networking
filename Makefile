CC := gcc
OUTDIR := out

## HELPERS
.PHONY: help
help:
	@echo Supported targets
	@cat $(MAKEFILE_LIST) | grep -e "^[\.a-zA-Z0-9_-]*: *.*## *" | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-35s\033[0m %s\n", $$1, $$2}'

.DEFAULT_GOAL := help

## TARGETS
build: clean ## build command
	$(CC) -g $(FLAGS) main.c -o $(OUTDIR)/main

.PHONY: run
run: build ## build and run post build
	(OUTDIR=${OUTDIR} \
		scripts/quick_run.sh)

build_no_stdlib: clean ## build without using stdlib
	$(CC) -g -nostdlib stubstart.S -o $(OUTDIR)/main main.c

# Don't forget to stop any running VPN serverce that uses a tun tap device!
post_build: ## setup tun0 interface with IP 192.168.0.1/24
	sudo setcap cap_net_admin=eip ${OUTDIR}/main
	sudo ip addr add 192.168.0.1/24 dev tun0
	sudo ip link set up dev tun0

ping: ## ping some address on tun0 interface
	ping -I tun0 192.168.0.2

del_tun_interface: ## delete the created tun0 interface
	sudo ip link delete tun0

tshark_on_tun0: ## sniff and parse the traffinc on tun0 interface
	sudo tshark -i tun0

clean: ## clean the generated build files
	rm -rf $(OUTDIR)/*