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

/*
 * This program supports loading firmware into a target USB device
 * that is discovered and referenced by the hotplug usb agent. It can
 * also do other useful things, like set the permissions of the device
 * and create a symbolic link for the benefit of applications that are
 * looking for the device.
 *
 *     -I <path>       -- Download this firmware (intel hex)
 *     -2              -- it's an FX2 (USB 2.0 capable) not FX
 *
 *     -L <path>       -- Create a symbolic link to the device.
 *     -m <mode>       -- Set the permissions on the device after download.
 *     -D <path>       -- Use this device, instead of $DEVICE
 *
 * This program is intended to be started by hotplug scripts in
 * response to a device appearing on the bus. It therefore also
 * expects these environment variables which are passed by hotplug to
 * its sub-scripts:
 *
 *     DEVICE=<path>
 *         This is the path to the device is /proc/bus/usb. It is the
 *         complete path to the device, that I can pass to open and
 *         manipulate as a USB device.
 */

# include  <stdlib.h>
# include  <stdio.h>
# include  <getopt.h>

# include  <sys/types.h>
# include  <sys/stat.h>
# include  <fcntl.h>

# include  "ezusb.h"


int main(int argc, char*argv[])
{
      const char*link_path = 0;
      const char*ihex_path = 0;
      const char*device_path = getenv("DEVICE");
      int fx2 = 0;
      mode_t mode = 0;
      int opt;

      while ((opt = getopt(argc, argv, "2D:I:L:m:")) != EOF) switch (opt) {

	  case '2':
	    fx2 = 1;
	    break;

	  case 'D':
	    device_path = optarg;
	    break;

	  case 'I':
	    ihex_path = optarg;
	    break;

	  case 'L':
	    link_path = optarg;
	    break;

	  case 'm':
	    mode = strtoul(optarg,0,0);
	    mode &= 0777;
	    break;
	  default:
	    goto usage;

      }

      if (!device_path) {
	    fputs ("no device specified!\n", stderr);
usage:
	    fputs ("usage: ", stderr);
	    fputs (argv [0], stderr);
	    fputs (" [-2] [-D devpath] [-I hexfile]", stderr);
	    fputs ("[-L link] [-m mode]\n", stderr);
	    fputs ("... [-D devpath] overrides DEVICE= in env\n", stderr);
	    return -1;
      }

      if (ihex_path) {
	    int fd = open(device_path, O_RDWR);
	    if (fd == -1) {
		  perror(device_path);
		  return -1;
	    }

	    ezusb_load_ihex(fd, ihex_path, fx2);
      }

      if (link_path) {
	    int rc = unlink(link_path);
	    rc = symlink(device_path, link_path);
	    if (rc == -1) {
		  perror(link_path);
		  return -1;
	    }
      }

      if (mode != 0) {
	    int rc = chmod(device_path, mode);
	    if (rc == -1) {
		  perror(link_path);
		  return -1;
	    }
      }

      if (!ihex_path && !link_path && !mode) {
	    fputs ("missing request! (firmware, link, or mode)\n", stderr);
	    return -1;
      }

      return 0;
}


/*
 * $Log$
 */

