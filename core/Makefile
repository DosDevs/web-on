all: webon

CLANG=/usr/local/bin/clang++10
COMPILE=$(CLANG) -std=c++20 -c
LINK=$(CLANG)

.SUFFIXES: .c++ .o

.c++.o:
	echo Compiling $?
	$(COMPILE) $?

webon: main.o httpd.o configuration.o worker.o request.o console-helper.o
	echo Linking $?
	$(LINK) $? -o $@ -lpthread

PHONY: clean

clean:
	rm -rfv webon *.o
