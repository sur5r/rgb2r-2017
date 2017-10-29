CC=cc65
AS=ca65
LD=cl65

CFLAGS= -t c128

all: image crctest

image: diskbench.d71

diskbench.d71: diskbench
	c1541 -format "diskbench",db d71 diskbench.d71
	touch data
	c1541 -attach diskbench.d71 -write diskbench
	c1541 -attach diskbench.d71 -write data data,s

%.s: %.c
	${CC} ${CFLAGS} -O1 $< -o $@

%.o: %.s
	${AS} ${CFLAGS} $< -o $@

diskbench.s: diskbench.c

diskbench: diskbench.o bcd2dec.o
	${LD} -t c128 $^ -o $@

crctest.s: crctest.c

crctest: crctest.o crc16.o bcd2dec.o
	${LD} -Ln crctest.lbl -t c128 $^ -o $@

clean:
	rm -f *.o crctest diskbench diskbench.d71 data *.lbl crctest.s diskbench.s
