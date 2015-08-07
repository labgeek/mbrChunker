
all:  mbrChunker

mbrChunker: Makefile mbrChunker.c
	gcc -g -Wall -o mbrChunker mbrChunker.c dd2vmdk.c mbr.c

clean:
	rm mbrChunker
