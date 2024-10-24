CFLAGS = -Wall -pedantic -g -std=c99
DEST = /usr/local/bin
BIN = server client

all: $(BIN)

install:
	cp client $(DEST)/bws_client
	cp server $(DEST)/bws_server

clean:
	rm -f $(BIN)
	rm -fr *.dSYM
