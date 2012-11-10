== uOFW installer ==

From ProCFW, from http://code.google.com/p/procfw/
Code is placed under GPLv3, as ProCFW.
It was patched so it only supports 6.60 and can run uOFW modules.

== Usage ==

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

PXE: run the installer using user + kernel exploits with patched Rebootex\_bin
Installer: install modules onto the NAND with the menu and run CFW or OFW and run rebootex (if asked)
Rebootex: load & run rebootex\_bin
Rebootex\_bin: patch loadcore to load user modules, add/rename custom modules of pspbtcnf and run real reboot.bin

- New modules:

SystemControl: patch loadcore/reboot (as Rebootex\_bin did) for executables ran into the VSH
Recovery: recovery menu
Satelite: VSH menu
usbdevice: enable connecting flash0/1/2 or UMD to USB
Vshctrl: to enter the recovery or satelite menu from VSH
Popcorn: POPS enabler
ISODrivers: the.. ISO drivers
Stargate: disable DRMs

== Running process ==

- PXE/Launcher: launcher PBP, starts the exploit -> kernel\_permission\_call()
  patches loadexec for it to run our own PXE/RebootEX, and then uses
  sceKernelExitGame() to run it
- PXE/RebootexPXE: patches reboot in order to add the fake "hen.prx" in the
  module list (when it is read, it actually reads SystemControlPXE) and patch
  reboot's loadcore to be able to run it
- PXE/SystemControlPXE: patches loadcore, modulemgr, memlmd in order to run
  unencrypted modules; patches sceLoadExec (at the time of running
  sceMediaSync) and vsh\_module in order to run the installer instead of them
[Nothing should be touched there]

- Installer: runs installer interface, copies & encrypts the modules to the
  NAND, and starts rebootex.prx (if the user chose to run the modified
  firmware) using normal module loading in start\_reboot(); also sets the
  arguments (options) it will pass to rebootex through 'argp' (module
  argument)
[May be changed to change list.txt reading and other writing to FW]

- Rebootex: patches loadexec to run our fake reboot.bin (Rebootex\_bin), copies
  arguments passed from the Installer to reboot memory, then runs Rebootex\_bin
[Should probably be left untouched, except for copying new options]

- Rebootex\_bin: patches lfatfs reading / real reboot in order to run our fake
  functions; \_UnpackBootConfig() adds the replaced & new modules (in list.txt
  or ProCFW modules like the important systemctrl) into the module list; a hook
  is started just before loadcore, and patches it, before running the patched
  loadcore
[Should be modified when running our own & when modifying options]

- Systemctrl: patch a lot of modules in order to be able to run non-signed
  EBOOTs from the VSH (loadcore, memlmd, interruptman, sysmem)
[Should be modified to remove our own modules patching & handle options]

