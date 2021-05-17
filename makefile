BINDIR=bin

all: src/*.c $(BINDIR)
	gcc src/*.c -Wall -o $(BINDIR)/ICONverter
$(BINDIR):
	mkdir bin