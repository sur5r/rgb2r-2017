CC=cc65
AS=ca65
LD=cl65

CFLAGS= -t c128

all: diskbench.d71 crctest relfile.d71


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

relfile.s: relfile.c

relfile: relfile.o
	${LD} -t c128 $^ -o $@

relfile.d71: relfile
	c1541 -format "relfile",rf d71 relfile.d71
	c1541 -attach relfile.d71 -write relfile

clean:
	rm -f bcd2dec.o crc16.o
	rm -f diskbench.d71 diskbench diskbench.o diskbench.s data
	rm -f crctest crctest.o crctest.s crctest.lbl
	rm -f relfile.d71 relfile relfile.o relfile.s
