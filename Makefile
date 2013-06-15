CC=cc
PACKAGE_NAME=mqtt-callerid
PACKAGE_VERSION=0.1
CFLAGS=-Wall -DPACKAGE_NAME='$(PACKAGE_NAME)' -DPACKAGE_VERSION='$(PACKAGE_VERSION)'
LDFLAGS=-lmosquitto


all: mqtt-callerid

mqtt-callerid: mqtt-callerid.o callerid.o
	$(CC) $(LDFLAGS) -o $@ $^

mqtt-callerid.o: mqtt-callerid.c callerid.h
	$(CC) $(CFLAGS) -c $<

callerid.o: callerid.c callerid.h
	$(CC) $(CFLAGS) -c $<

test: callerid.o callerid.h
	$(CC) $(CFLAGS) -c test-callerid.c
	$(CC) $(LDFLAGS) -o test-callerid test-callerid.o callerid.o
	./test-callerid

clean:
	rm -f *.o mqtt-callerid test-callerid

dist:
	distdir='$(PACKAGE_NAME)-$(PACKAGE_VERSION)'; mkdir $$distdir || exit 1; \
	list=`git ls-files`; for file in $$list; do \
		cp -pR $$file $$distdir || exit 1; \
	done; \
	tar -zcf $$distdir.tar.gz $$distdir; \
	rm -fr $$distdir


.PHONY: all test clean dist
