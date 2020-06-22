all: webon

CLANG=/usr/local/bin/clang10
COMPILE=$(CLANG) -std=c++20 -c
LINK=$(CLANG) -lc++

.SUFFIXES: .c++ .o

.c++.o:
	echo Compiling $? using $(CLANG)
	$(COMPILE) $?

webon: main.o
	echo Linking $?
	$(LINK) $? -o $@

PHONY: clean

clean:
	rm -rf *.o
