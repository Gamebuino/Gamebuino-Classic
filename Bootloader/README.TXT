The folder contains the files required to burn and test the prototype Gamebuino bootloader.

==================================
== How to Build the Bootloader ==
==================================

This project has only been tested with the latest version of the official Arduino Windows toolchain i.e. 1.0.5. If you are running a
later version then you'll need to change the line in build.bat that adds it to the path:

		set PATH=%PATH%;C:\arduino-1.0.5\hardware\tools\avr\utils\bin

You'll also need to change the first line in the makefile:

		ARDUINO_DIR = C:/arduino-1.0.5

Now run build.bat, the project will be built in the obj subfolder and if successful the hex file (gamebuino_boot.hex) will be
copied to the main folder and displayed in a directory listing.

================================
== How to Burn the Bootloader ==
================================

1) Copy the contents of this folder into a new folder in C:\arduino-1.0.5\hardware\arduino\bootloaders\gamebuino_boot
2) Connect a programmer (or Arduino as ISP) to the target Gamebuino device.
3) The official Arduino IDE programs Uno with the optiboot bootloader which is only 512 bytes long. We need a larger bootloader
   for MMC updates so we need to tell the IDE to set the fuses to accommodate this. The fuse setting chosen here is exactly the
   same as for optiboot but with the boot sector set to 4kb (see http://www.engbedded.com/fusecalc); this is for dev/test
   purposes only and will be reduced to 2kb in the final build.

   Open the C:\arduino-1.0.5\hardware\arduino\boards.txt file and add the following section:

	gamebuino.name=Gamebuino
	gamebuino.upload.protocol=arduino
	gamebuino.upload.maximum_size=32256
	gamebuino.upload.speed=115200
	gamebuino.bootloader.low_fuses=0xff
	gamebuino.bootloader.high_fuses=0xd8
	gamebuino.bootloader.extended_fuses=0x05
	gamebuino.bootloader.path=gamebuino_boot
	gamebuino.bootloader.file=gamebuino_boot.hex
	gamebuino.bootloader.unlock_bits=0x3F
	gamebuino.bootloader.lock_bits=0x0F
	gamebuino.build.mcu=atmega328p
	gamebuino.build.f_cpu=16000000L
	gamebuino.build.core=arduino
	gamebuino.build.variant=standard

4) Burn the bootloader into the Gamebuino:
	- Restart the Arduino IDE
	- Select Tools -> Board -> Gambuino
	- Select your programmer's COM port from Tools -> Serial Port
	- Select the appropriate programmer e.g. Tools -> Programmer -> Arduino as ISP
	- Select Tools -> Burn Bootloader

===============================
== How to Use the Bootloader ==
===============================

The Gamebuino bootloader is based on optiboot and should behave identically under normal operation. However, if the Gamebuino C button
is pressed down upon power-up/reboot then it will try to load and execute LOADER.HEX from the SD card.

The "Loader" subfolder contains a test loader showing how to invoke the bootloader's flash and execute code using a custom filename:

	#define load_game (*((void(*)(char*))0x7ffc))
	load_game("BLINK");

Note that the filename must be all upper case and must have the .HEX extension. The jump table vector for this routine is fixed at
offset 0x7ffc and should remain so in future to maintain backwards compatibility with previous bootloader versions.

=============
== Credits ==
=============
- Bootloader code based on the official "optiboot" bootloader for Arduino Uno (located in hardware\arduino\bootloaders\optiboot).
- MMC serialization/flash code by thseiler's "2boots" MMC booter (https://github.com/thseiler/embedded/tree/master/avr/2boots)
- Gamebuino-specific modications and build architecture by Mark Feldman (accounts@ppl-pilot.com)
- Gamebuino handheld designed by rodot (http://gamebuino.com/)
