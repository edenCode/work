include config.mak


all: clean
	make -C cryptolib
	make -C SSL_server
	make -C SSL_client
clean:
	make -C cryptolib clean
	make -C SSL_server clean
	make -C SSL_client clean
