/*
 * Copyright (c) 2001 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#ident "$Id$"

# include  <stdio.h>
# include  <errno.h>
# include  <assert.h>

# include  <linux/ioctl.h>
# include  <linux/usbdevice_fs.h>

/*
 * This file contains functions for downloading firmware into Cypress
 * EZ-USB devices. The chip uses control endpoint 0 and vendor
 * specific commands to support writing into the on-chip SRAM. It also
 * supports writing into the CPUCS register, and this is how we reset
 * the processor.
 *
 * These Cypress devices are 8-bit 8051 based microcontrollers with
 * special support for USB I/O.  They come in several packages, and
 * some can be set up with external memory when device costs allow.
 */


/*
 * The ezusb_poke function writes a stretch of memory to the target
 * device. The device has already been opened, and the address chosen
 * by the input source.
 *
 * Incidentally, all the O/S specific parts are in this function.
 */

# define RETRY_LIMIT 5

static int ezusb_poke(int fd, unsigned addr,
		      const void*data, size_t len)
{
      int rc;
      struct usbdevfs_ctrltransfer ctrl;
      const unsigned char*bytes = (const unsigned char*)data;

      while (len > 0) {
	    unsigned retry;
	    unsigned char buf[64];
	    unsigned trans = len;

	      /* Only send 64 bytes at a time. */
	    if (trans > sizeof buf)
		  trans = sizeof buf;

	    memcpy(buf, bytes, trans);

	      /* request type 0x40 is Vendor Request OUT */
	    ctrl.requesttype = 0x40;
	      /* bRequest 0xa0 is Firmware load */
	    ctrl.request = 0xa0;
	      /* Put the target address in the value field */
	    ctrl.value   = addr;
	      /* All the data in the buffer go to the target. */
	    ctrl.length  = trans;
	    ctrl.index = 0;
	    ctrl.timeout = 3000;
	    ctrl.data = buf;

	      /* Try this a couple times. Control messages are not
		 NAKed (then are just dropped) so I only time out when
		 there is a problem. */
	    retry = 0;
	    while ((rc = ioctl(fd, USBDEVFS_CONTROL, &ctrl)) == -1) {
		  if (errno != ETIMEDOUT) {
			perror ("usb vendor control request");
			break;
		  }

		  if (retry >= RETRY_LIMIT)
			break;

		  retry += 1;
	    }

	    if (rc < 0)
		  return rc;

	    bytes += trans;
	    addr += trans;
	    len -= trans;
      }

      return 0;
}

static const char need2stage [] =
    "need two stage loader to load memory at %#04x\n";

/*
 * Load an intel HEX file into the target. The fd is the open USB
 * device, and the path is the name of the source file. Open the file,
 * interpret the bytes and write as I go.
 */
int ezusb_load_ihex(int fd, const char*path, int fx2)
{
      unsigned char data[512];
      FILE*image;
      int rc;
      unsigned short cpucs_addr;

      if (fx2)
	    cpucs_addr = 0xe600;
      else
	    cpucs_addr = 0x7f92;

      image = fopen(path, "r");
      if (image == 0) {
	    fprintf(stderr, "%s: unable to open for input.\n", path);
	    return -2;
      }


	/* This writes the CPUCS register on the target device to hold
	   the CPU reset. Do this first, so that I'm free to write its
	   program data later. */
      { unsigned char cpucs = 0x01;
        ezusb_poke(fd, cpucs_addr, &cpucs, 1);
      }


	/* Now read the input file as an IHEX file, and write the data
	   into the target as I go. */
      for (;;) {
	    char buf[1024], *cp;
	    char tmp;
	    unsigned len, type;
	    unsigned idx;
	    unsigned off;

	    cp = fgets(buf, sizeof buf, image);
	    if (cp == 0)
		  break;

	    if (buf[0] != ':') {
		  fprintf(stderr, "not an ihex record: %s", buf);
		  return -2;
	    }

	      /* Read the length field */
	    tmp = buf[3];
	    buf[3] = 0;
	    len = strtoul(buf+1, 0, 16);
	    buf[3] = tmp;

	      /* Read the target offset */
	    tmp = buf[7];
	    buf[7] = 0;
	    off = strtoul(buf+3, 0, 16);
	    buf[7] = tmp;

	      /* Read the record type */
	    tmp = buf[9];
	    buf[9] = 0;
	    type = strtoul(buf+7, 0, 16);
	    buf[9] = tmp;

	      /* If this is an EOF record, then break. */
	    if (type ==1)
		  break;

	    if (type != 0) {
		  fprintf(stderr, "unsupported record type: %u\n", type);
		  return -3;
	    }

	    if ((len * 2) + 11 >= strlen(buf)) {
		  fprintf(stderr, "record too short?\n");
		  return -4;
	    }

	    /* Sanity check: this is only a first-stage loader, we can only
	     * load some parts of the on-chip SRAM.  Loading from a "big" I2C
	     * serial ROM (more than USB vid/pid) has the same constraints.
	     *
	     * Bigger programs use "real ROM", or need two-stage loaders that
	     * know the physical memory model in use.  Such models range from
	     * simple "64K I+D" or "64K I + 64K D", to bank switching setups.
	     */
	    if (fx2) {
		/* 1st 8KB for data/program, 0x0000-0x1fff */
		if (off <= 0x1fff) {
		    if ((off + len) > 0x2000) {
			fprintf(stderr, need2stage, off);
			return -5;
		    } /* else OK */
		/* and 512 for data, 0xe000-0xe1ff */
		} else if (off >= 0xe000 && off <= 0xe1ff) {
		    if ((off + len) > 0xe200) {
			fprintf(stderr, need2stage, off);
			return -5;
		    } /* else OK */
		} else {
		    fprintf(stderr, need2stage, off);
		    return -5;
		}
	    } else {
		/* with 8KB RAM, 0x0000-0x1b3f can be written
		 * here; we can't tell if it's a 4KB device here
		 */
		if (off <= 0x1b3f) {
		    if ((off + len) > 0x1b40) {
			fprintf(stderr, need2stage, off);
			return -5;
		    } /* else OK */
		} else {
		    fprintf(stderr, need2stage, off);
		    return -5;
		}
	    }

	    for (idx = 0, cp = buf+9 ;  idx < len ;  idx += 1, cp += 2) {
		  tmp = cp[2];
		  cp[2] = 0;
		  data[idx] = strtoul(cp, 0, 16);
		  cp[2] = tmp;
	    }

	    rc = ezusb_poke(fd, off, data, len);
	    if (rc < 0) {
		  fprintf(stderr, "failed to write data to device\n");
		  return -1;
	    }
      }


	/* This writes the CPUCS register on the target device to
	   release the host reset. After this, the processor is free
	   to renumerate, or whatever. */
      { unsigned char cpucs = 0x00;
        ezusb_poke(fd, cpucs_addr, &cpucs, 1);
      }


      return 0;
}


/*
 * $Log$
 * Revision 1.1  2001/06/12 00:00:50  stevewilliams
 *  Added the fxload program.
 *  Rework root makefile and hotplug.spec to install in prefix
 *  location without need of spec file for install.
 *
 */

