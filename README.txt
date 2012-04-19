From ProCFW, from http://code.google.com/p/procfw/
Code is placed under GPLv3, as ProCFW.
It was patched so it only supports 6.60 and can run uOFW modules.

== Directories ==

- Common:

Common: common utility functions
CrossFW: cross-firmware functions (quite useless now actually)
Imports: needed module imports

- Installer:

PXE: run the installer using user + kernel exploits with patched Rebootex_bin
Installer: install modules onto the NAND with the menu and run CFW or OFW and run rebootex (if asked)
Rebootex: load & run rebootex_bin
Rebootex_bin: patch loadcore to load user modules, add/rename custom modules of pspbtcnf and run real reboot.bin

- New modules:

SystemControl: patch loadcore/reboot (as Rebootex_bin did) for executables ran into the VSH
Recovery: recovery menu
Satelite: VSH menu
usbdevice: enable connecting flash0/1/2 or UMD to USB
Vshctrl: to enter the recovery or satelite menu from VSH
Popcorn: POPS enabler
ISODrivers: the.. ISO drivers
Stargate: disable DRMs

== Installing ==

You need a 6.60 OFW or CFW (all the 01g-09g should work; it's brick-free).
Run: make
Copy the contents of the "dist" directory at the root of your memory stick
Create a directory named uofw at the root of your memory stick and place the uofw modules there
Run the uOFW installer from the VSH!

