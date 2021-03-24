CC = gcc
OUTDIR = out

help:	## Show this help.
	@fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep | sed -e 's/\\$$//' | sed -e 's/##//'

build: clean	## build command
	$(CC) -g $(FLAGS) main.c -o $(OUTDIR)/main

build_no_stdlib: clean	## build without using stdlib
	$(CC) -g -nostdlib stubstart.S -o $(OUTDIR)/main main.c

clean:	## clean the generated build files
	rm -rf $(OUTDIR)/*