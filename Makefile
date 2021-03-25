CC = gcc
OUTDIR = out

help:	## Show this help.
	@fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep | sed -e 's/\\$$//' | sed -e 's/##//'

build: clean	## build command
	$(CC) -g $(FLAGS) main.c -o $(OUTDIR)/main

build_no_stdlib: clean	## build without using stdlib
	$(CC) -g -nostdlib stubstart.S -o $(OUTDIR)/main main.c

set_cap_net_admin:
	sudo setcap cap_net_admin=eip $(OUTDIR)/main
	sudo ip addr add 192.168.0.1/24 dev tun0
	sudo ip link set up dev tun0

clean_interface:
	sudo ip link delete tun0

clean:	## clean the generated build files
	rm -rf $(OUTDIR)/*