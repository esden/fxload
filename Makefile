
prefix = /
exec_prefix = ${prefix}
sbindir = ${exec_prefix}/sbin
mandir = ${prefix}/usr/share/man

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}


CFLAGS = -O -Wall

PROG = fxload

all: $(PROG)

O = main.o ezusb.o


$(PROG): $O
	$(CC) -o $(PROG) $O

clean:
	rm -f *.o *~ fxload

install:
	$(INSTALL_PROGRAM) fxload $(sbindir)/fxload
	$(INSTALL_PROGRAM) -m 0644 fxload.8 $(mandir)/man8/fxload.8

%.o: %.c
	$(CC) -c $(CFLAGS)  $< -o $@

main.o: main.c ezusb.h
ezusb.o: ezusb.c ezusb.h
