
prefix = /
exec_prefix = ${prefix}
sbindir = ${exec_prefix}/sbin

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}


CFLAGS = -O

PROG = fxload

all: $(PROG)

O = main.o ezusb.o


$(PROG): $O
	$(CC) -o $(PROG) $O

clean:
	rm -f *.o *~ fxload

install: $(sbindir)/fxload

$(sbindir)/fxload: fxload
	$(INSTALL_PROGRAM) fxload $(sbindir)/fxload

%.o: %.c
	$(CC) -c $(CFLAGS)  $< -o $@

main.o: main.c ezusb.h
ezusb.o: ezusb.c ezusb.h
