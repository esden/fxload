#ifndef __ezusb_H
#define __ezusb_H
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
 * This function loads into the opened EZUSB device the firmware in
 * the given file. The file is assumed to be in Intel HEX format, and
 * is loaded into the target memory literally.  If fx2 is set, uses
 * different reset commands.
 *
 * The target processor is reset as part of this load.
 */
extern int ezusb_load_ihex(int dev, const char*path, int fx2);

/*
 * $Log$
 */
#endif
