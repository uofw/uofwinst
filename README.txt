From ProCFW, from http://code.google.com/p/procfw/
Code is placed under GPLv3, as ProCFW.
It was patched so it only supports 6.60 and can run uOFW modules.

== uOFW installer usage ==

You need any 6.60 firmware, on any model.

* Copy the contents of the "dist" directory at the root of your memory stick.
* Create a directory named "uofw" (without the quotation marks) at the root of your memory stick.
* Put the modules into the uofw directory you created on your memory stick.
* Create a file named list.txt in the uofw directory and fill it with this syntax:

; Lines starting with ; are comments
[replace]
; modules to replace with your own version (don't put the "<>" in the file)
; they must be with the same name in the uofw directory and will replace in pspbtcnf the module at "/kd/<module>.prx"
<module>.prx
[add]
; insert modules
; here, <new_module>.prx is inserted right before <next_module>.prx in pspbtcnf
<new_module>.prx <next_module>.prx

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

