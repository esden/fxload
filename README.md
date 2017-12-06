FXLOAD UTILITY
==============

This program is conveniently able to download firmware into FX, FX2,
and FX2LP EZ-USB devices, as well as the original AnchorChips EZ-USB.
It is intended to be invoked by hotplug scripts when the unprogrammed
device appears on the bus.

Primarily as an aid for developers, this can also be used to update
firmware on devices which boot from I2C serial EEPROMs.  For that
use, as well as downloading firmware to all other off-chip memory,
a second stage loader must first be downloaded.

The distribution includes "a3load.hex", which is a simple second stage
loader that works with all the EZ-USB products listed above.  If you
want to write to an EEPROM, you can use the appropriate version of the
`Vend_Ax` code provided with the Cypress developer kit.


UPDATES
=======

See the download page at http://linux-hotplug.sourceforge.net for the
latest release.  The CVS repository there holds the most current version
of this software.  The web site shows some ways to use "fxload" in
conjunction with USB hotplugging, to load device firmware.

Post any bug reports to the linux-hotplug-devel or linux-usb-devel lists.

If you modify the code, the GPL requires that you make your updates
generally available (under most circumstances).  Providing them in the
form of patches against the current CVS (`cvs diff -u`), or the last
release (`diff -u old new`) is most useful, since otherwise it's awkward
to integrate such changes into the standard distribution.

EXAMPLES
========

Flash a custom VID:PID into a device EEPROM:
```
./fxload -D /dev/bus/usb/aaa/bbb -c 0x01 -s Vend_Ax.hex -t fx2lp -d xxxx:xxxx
```

You can retreive the correct path by reading the first numbers in front of the
device entry provided in `lsusb` in front of the FX2 device you are trying to
program.

Flash a custom firmware into a device EEPROM usind default VID:PID:
```
./fxload -D /dev/bus/usb/aaa/bbb -I firmware.hex -c 0x01 -s Vend_Ax.hex -t fx2lp
```

Flash a custom firmware into a device EEPROM using custom VID:PID:
```
./fxload -D /dev/bus/usb/aaa/bbb -I firmware.hex -c 0x01 -s Vend_Ax.hex -t fx2lp -d PIDx:VIDx
```
