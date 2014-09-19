alienfx
=======

Control AlienFX lighting on Alienware Aurora computers running Linux


CAVEAT
------

This code is essentially a quick hack, with numerous stylistic
inconsistances and other issues.  The "#define"s are legacy code - the
command IDs could certainly be an enum, etc.  Variable names are also
inconsistent in places, the Command function could be split up, more
chipset commands could be implemented; the list goes on.  My objective was
solely to get uptime(1) to be reflected in my case colors, which limited
the project's thoroughness.  Despite this, I hope someone else will find it
useful.

INTRO
-----

This software, alienfx(1) and alienfx-uptime(1), are utilities for
controlling the AlienWare AlienFX LED case lighting system as seen on the
Aurora line, generally sold by Dell, from the linux command line.  Support
status:

   Aurora (non-ALX):   works
         Aurora ALX:   some testing
             Area51:   untested
       Allpowerfull:   untested (and I'm skeptical of the the spelling)

I don't have the lighting info for any but the Aurora non-ALX version tested.
I especially don't know any details about the Aurora ALX AlienFX chipset.
Code diffs to update them the lighting descriptions are appreciated.
If you email me anything about this software, please prefix "alienfx:" to
the subject line.

It is not from, nor supported by, AlienWare in any way.  AlienWare doesn't
even provide protocol documentation, so be aware that everything done here
has been constructed via reverse engineering or guesswork.

The AlienFX subsystem appears to the host computer as a USB-connected device,
and takes simple commands, a subset of which are implement by this program.
Instructions are also provided here for setting up the device with
appropriate permissions and a pathname under Ubuntu Linux.

COMPILATION
-----------

This alienfx program relies on a number of packages, of which only the
libusb package generally ends up being tricky to install.   The key line
from the source to be aware of is:

   #include <libusb-1.0/libusb.h>

which refers to the default layout of the following Ubuntu/Debian packages:

   libusb-1.0-0      2:1.0.1-1 userspace USB programming library
   libusb-1.0-0-dev  2:1.0.1-1 userspace USB programming library development

So, on Ubuntu, the following should install the specific version needed:

   apt-get install libusb-1.0-0-dev

To begin setup for compilation:

   ./configure

Optionally, specify the target directory area with --prefix=/usr/local
or whatever is desired.

FINDING THE DEVICE
------------------

As root:

   lsusb -v

...should return a chunk containing text resembling that below.  Note
particularly the idVender 0x187c which is AlienWare, and the idProduct 0x0513
GS Desktop which identifies the Aurora version of the AlienFX lighting.

    Bus 007 Device 002: ID 187c:0513  
    Device Descriptor:
      bLength                18
      bDescriptorType         1
      bcdUSB               1.10
      bDeviceClass            0 (Defined at Interface level)
      bDeviceSubClass         0 
      bDeviceProtocol         0 
      bMaxPacketSize0        64
      idVendor           0x187c 
      idProduct          0x0513 
      bcdDevice            0.00
      iManufacturer           1 Alienware
      iProduct                2 G2 Desktop
      iSerial                 3 1.1.32
      bNumConfigurations      1
      Configuration Descriptor:
        bLength                 9
        bDescriptorType         2
        wTotalLength           41
        bNumInterfaces          1
        bConfigurationValue     1
        iConfiguration          0 
        bmAttributes         0x80
          (Bus Powered)
        MaxPower               64mA
        Interface Descriptor:
          bLength                 9
          bDescriptorType         4
          bInterfaceNumber        0
          bAlternateSetting       0
          bNumEndpoints           1
          bInterfaceClass         3 Human Interface Device
          bInterfaceSubClass      0 No Subclass
          bInterfaceProtocol      0 None
          iInterface              0 
            HID Device Descriptor:
              bLength                 9
              bDescriptorType        33
              bcdHID               1.01
              bCountryCode            0 Not supported
              bNumDescriptors         1
              bDescriptorType        34 Report
              wDescriptorLength      54
             Report Descriptors: 
               ** UNAVAILABLE **
          Endpoint Descriptor:
            bLength                 7
            bDescriptorType         5
            bEndpointAddress     0x81  EP 1 IN
            bmAttributes            3
              Transfer Type            Interrupt
              Synch Type               None
              Usage Type               Data
            wMaxPacketSize     0x000a  1x 10 bytes
            bInterval              10
    Device Status:     0x0000
      (Bus Powered)

The idVendor and idProduct are used in the next step of the process.

INSTALLATION
------------

In Linux with UDEV, you might want to allow users to access the AlienFX device
by adding a file /etc/udev/rules.d/alienfx.rules with:

    ATTR{idVendor}=="187c", ATTR{idProduct}=="0513", MODE="0660", GROUP="adm", SYMLINK+="alienfx"

The above example allows access to the Aurora AlienFX for anyone in group
"adm", and provides an extra symlink in /dev/alienfx (which this package
doesn't use).  Modify the ids as needed for different AlienFX hardware.
OWNER can be used too, but a system group makes sense if the LEDs are going
to be active even without a user logged in.

For testing, you can dig up the devpath with:

    udevadm trigger  --verbose --dry-run --attr-match=idVendor=187c

(I'd actually used --attr-match=idProduct=0513 instead of the idVendor)

And then exercise the new file's content by sticking the result in place
of the $devpath in the following command,

    udevadm test $devpath

Or if you're just lazy, copy and paste this combo:

    udevadm test `udevadm trigger --verbose --dry-run --attr-match=idVendor=187c`

If it works, you'll see correct permissions and ownership on whatever your
system has instead of /dev/bus/usb/007/002 (or whatever), and can see the
actual path by looking at the /dev/alienfx symbolic link.  Or, briefly:

   $ ls -lL /dev/alienfx 
   crw-rw---- 1 root adm 189, 769 2010-11-01 17:33 /dev/alienfx

This code only implements direct access, rather than a sweet little daemon
or something to field multiple connected clients or something.

It's horribly annoying that changing the color incurs something like a
two-second delay.  This kind of hardware choice makes it impossible to do
smooth/fast crossfades, interesting flickers, and so on in a truly dynamic
way.  Sad.  There's some question ask to whether 256 levels of r, g, and b
are actually supported or just quantized to every 16 or so.  There's no
documentation online for the protocol either, so it's difficult to 
determin whether the apparent misfeatures are actually hardware limitations.

USE
---

The alienfx-uptime effect is quite nice. :-)

Good luck!

NOTES ABOUT THE PACKAGING
-------------------------

The following files were actually pulled in from a larger build system:

* GNUmakecore.in
* config.h.in
* configure
* configure.ac

There are included in the repository in the hopes they'll make it easy to
build alienfx without too much effort, but replacing the GNUmakefile outright
with a simpler approach would also work.

The normal product of "make dist" is generally available at the URL below, and
includes updating files from the surrounding build system:

    https://www.talisman.org/~erlkonig/software/pub/alienfx.tar.gz

THANKS
------

Some of the materials leading to this project were drawn from

* Benjamin Thaut - http://3d.benjamin-thaut.de/
* The AlienFXLite project
* Stefan Saraev - who threw my code on github before I did, added m11x,
  and noted a timing issue.
