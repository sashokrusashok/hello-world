CC = gcc
CFLAGS = -pedantic -Wall -g

all:	
		$(CC) client_tufilter.c $(CFLAGS) -o tufilter

test:
		sudo ./tufilter --transport tcp --ip 172.25.23.4 --port 4357 --filter enable
		sudo ./tufilter --transport udp --ip 192.168.23.4 --port 7777 --filter enable
		sudo ./tufilter --transport udp --ip 10.0.2.15 --port 7778 --filter enable
		sudo ./tufilter --transport udp  --port 7778 --filter enable
		sudo ./tufilter --transport tcp --ip 192.168.23.5 --port 9999 --filter enable
		sudo ./tufilter --transport tcp --port 7777 --filter enable
		sudo ./tufilter --transport tcp --ip 10.0.2.15 --filter enable

show:
		sudo ./tufilter --show

clean:
		sudo rm tufilter