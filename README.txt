
FXLOAD UTILITY

This program is conveniently able to download firmware into FX and FX2
EZ-USB devices, as well as the original AnchorChips EZ-USB.  It is
intended to be invoked by hotplug scripts when the unprogrammed device
appears on the bus.

Primarily as an aid for developers, this can also be used to update
firmware on devices which boot from I2C serial EEPROMs.  For that
use, as well as downloading firmware to all other off-chip memory,
a second stage loader must first be downloaded.

The distribution includes "a3load.hex", which is a simple second stage
loader that works with all the EZ-USB products listed above.  If you
want to write to an EEPROM, you can use the appropriate version of the
"Vend_Ax" code provided with the Cypress developer kit.


UPDATES

See the download page at http://linux-hotplug.sourceforge.net for the
latest release.  The CVS repository there holds the most current version
of this software.  The web site shows some ways to use "fxload" in
conjunction with USB hotplugging, to load device firmware.

Post any bug reports to the linux-hotplug-devel or linux-usb-devel lists.

