Summary:		EZ-USB utility program
Name:			fxload
Version:		VERCODE
Release:		1
Group:			Applications/System
License:		GPL
Url:			http://linux-hotplug.sourceforge.net/
Source0:		%{name}-%{version}.tar.gz

%description
This package contains utilities for downloading firmware to EZ-USB devices.
EZ-USB devices use 8051-based microcontrollers that have been enhanced with
registers, buffers, and other device-side support for USB transactions.

It currently supports devices based on the Anchorchips EZ-USB, as well as the
Cypress EZ-USB FX (which is almost completely source compatible) and EZ-USB FX2
(which is not).  All of these support full speed (12 Mbit/sec) transfers.  The
FX2 also supports high speed (480 Mbit/s) transfers, introduced in USB 2.0.

This version of FXLOAD supports optional use of two-stage loading, where 
special device firmware is used to support writing into off-chip memory such
as RAM (when firmware neeeds more than about 8 KBytes of code and data) or,
for firmware development, I2C serial EEPROM.

%prep
%setup -q

%build
# Build any compiled programs in the source tree.
make all CFLAGS="%optflags -DFXLOAD_VERSION=\\\"%version\\\""

%install
make prefix=%buildroot install

# --
# The utils package contains utility programs that certain devices
# might need along with the core hotplug in order to boot up the
# device in question.
%files
%defattr(-,root,root)
/sbin/fxload


%changelog
* Mon Apr 01 2002 David Brownell <dbrownell@users.sourceforge.net>
- fixed/updated RPM build and Makefile
- Makefile automatically embeds version into specfile and binary
- fxload handles 2nd stage loader

* Mon Jan 21 2002 David Brownell <dbrownell@users.sourceforge.net>
- update fxload rpm description.  also, manpage now installs.

* Mon Jan 14 2002 Greg Kroah-Hartman <greg@kroah.com>
- split fxload specific stuff out of the base hotplug.spec file
  to keep the hotplug core arch independant

* Mon Jun 11 2001 Steve Williams <steve@icarus.com>
- The install process is now in the comon makefile, so that non-
  rpm installs can work. This spec file thus invokes that install.

* Fri Jun 8 2001 Steve Williams <steve@icarus.com>
- added the /var/run/usb directory to spec file

* Tue Apr 24 2001 Greg Kroah-Hartman <greg@kroah.com>
- added the hotplug.8 manpage written by Fumitoshi UKAI <ukai@debian.or.jp>

* Fri Mar 2 2001 Greg Kroah-Hartman <greg@kroah.com>
- tweaked the post and preun sections to fix problem of hotplug
  not starting automatically when the package is upgraded.

* Wed Feb 28 2001 Greg Kroah-Hartman <greg@kroah.com>
- 2001_02_28 release

* Wed Feb 14 2001 Greg Kroah-Hartman <greg@kroah.com>
- 2001_02_14 release

* Wed Jan 17 2001 Greg Kroah-Hartman <greg@kroah.com>
- changed specfile based on Chmouel Boudjnah's <chmouel@mandrakesoft.com> comments.

* Tue Jan 16 2001 Greg Kroah-Hartman <greg@kroah.com>
- tweaked the file locations due to the change in the tarball structure.
- 2001_01_16 release

* Mon Jan 15 2001 Greg Kroah-Hartman <greg@kroah.com>
- First cut at a spec file for the hotplug scripts.
- added patch to usb.rc to allow chkconfig to install and remove it.

