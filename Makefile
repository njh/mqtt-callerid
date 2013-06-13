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

clean:
	rm -f *.o mqtt-callerid

dist:
	distdir='$(PACKAGE)-$(VERSION)'; mkdir $$distdir || exit 1; \
	list=`git ls-files`; for file in $$list; do \
		cp -pR $$file $$distdir || exit 1; \
	done; \
	tar -zcf $$distdir.tar.gz $$distdir; \
	rm -fr $$distdir


.PHONY: all clean dist
