#Porting grbl to Freescale's K20

Notes, as a possible blogpost/technical report, on porting [grbl](https://github.com/grbl/grbl) to the [Teensy 3.0 and 3.1](http://www.pjrc.com/store/teensy3.html) Freescale K20 ARM development boards.

## Toolchain

   The standard teensy 3.0 toolchain is setup in such a way as to be unsuitable for programming beyond the confines of the Arduino API. Although modifying grbl to use the abstraction layer provided by the teensyduino code is probably possible (and may even result in something functional), it seems to violate the spirit of grbl. Although the reference hardware platform is the prototypical Arduino, grbl very explicitly doesn't use the Arduino environment!

   * Most significant reason not to use the standard environment: they provide implementations of ISRs that we need to run our code in. Less drastic solutions include deleting them from the source (easy, reduced code size for unused ISRs), or modifying their declarations to turn them into weak symbols (compatible with libraries that do require an ISR that we don't implement, may be able to push the fix upstream).

   * However, distributing the modifications is as much work as distributing a stripped-down environment, and only gives us access to a few functions that grbl doesn't use.

   * this then requires a minimalist implementation of pins_teensy.c, some modifications to mk20*.c, and whatever files implement all of the USB stuff.

   * init.c is actually pretty useless and could be removed, or refactored into a bunch of independent routines that are directly called from main.

## Processor Differences

###Timers

###IO

  * clean, although we do want a bunch of macros to make selecting bits simpler.

###EEPROM

  * K20s have FlexRAM, not eeprom. Interface is different - implemented in teensys eeprom.c.
  * In particular, we very much don't want to get in a situation involving unaligned reads or writes.
  * This, of course, isn't an issue on an 8-bit processor, so grbl's implementation is full of single-byte reads/writes.
  * Configuring the FlexRAM is quite odd, and requires executing some code from RAM.
  * In the interest of brevity, we provide two functions - aligned *n*-word write with 32-bit checksum, and aligned *n*-word read with a checksum. The K20s have hardware support for calculating CRCs, so we use that and calculate a CRC_32C in place of grbl's original 8-bit checksum. This level of redundancy probably isn't required, given the low expected number of writes and superior reliability (32K vs 10K cycles), but the original code is paranoid enough to bother with it, so why not.
  * The restricted API will require rethinking how GRBL deals with NVM, but that's fine.
  * Documentation for the FlexRAM is confusing and hard to parse, so I'm relying on the original toolchain's function for configuring it. Documentation for the CRC module is much better - C. 30 of the reference manual.    
	
###USB Serial Communication
 
  * We want to use the K20 as a cdc device. Reference implementation is in usb_serial.c.
  * Also need to initialize the usb subsystem, which is usb_dev.c and requires usb_desc.c. This requires usb_names.c and nonstd.c, etc. Quite a large dependency set.
  * This is much more complicated than I want to deal with right now. Cleaning it up and specializing this is a possibility, but a clean re-implementation is pointless, at least during this quarter.

## Hardware Configuration

  * (Possibly conflicting, and roughly prioritized) goals for assigning pins:
    * Easy programming interface - stepper pins are all in the same port, etc.
    * Still gives access to peripherals - we need: SPI, I²C, ADC, FlexTimer quad decoder.
    * Logical to actually configure wiring for teensy3.x. SMD pins on bottom are thus not desirable.
    * Extensible to a 4-axes (minimum, 5 or 6 would be cooler) configuration.

  * grbl requires step/dir for each axes, limits, stepper enable, spindle enable, spindle direction, reset, feed hold,  cycle start/resume,  coolant enable.
    * Of these, *n*-step pins, *n*-dir pins, and *n*-limits more-or-less need to be on common ports. The rest of the IO can be on random things.
   
  * SPI happens on port D - D1, D2, D3 are {SCLK, MOSI, MISO}, the rest can be used as chip-select or GPIO. On Teensy, these are pins 14, 7, 8.
  * D5 and D6 are ADC - pins 20 and 21. 
  * Port C gets stepper control - C0-C11 give us 6-axes step and direction.
    * Pin 9,  10, 11 (C3,C4,C6) are step for x,y,z
    * Pin 12, 13, 15 (C7,C5,C0) are dir for x,y,z
    * Pins C1, C2, C8, C9, C10, C11 are random IO, or 6-axis expansion.

  * Port B is 6-limit switches, and 2 GPIO.
    * Pins 16,17,18 are limit switches for x,y,z
    * Pins 0, 1, 25 (B16,B17,B19) are reset, feed hold, cycle start/resume

  * Spindle enable is pin 3 (12), spindle direction is 4 (A13)
  * Flood coolant is 5 (D7), mist coolant is 6 (D4)

  * 5 more gpio are required - the rest of port A and E are dedicated to that (6 total), as is the rest of port D. 

## Per-File Notes

### main.c

  * Only needed to change a few includes in order to get a clean compile. All avr specific includes are removed (interrupt.h, pgmspace.h), and
      stdint.h is inserted. Also, main explicitly enables interrupts after the first round of initialization - probably not needed.

### spindle.c

  * Very simple, toggles two pins - direction, speed
  * Sets the pattern for declaring the pin mapping - define the gpio register as a macro parameter over the operation. 

### coolant.c

  * Again, just as simple as spindle.c.

### stepper.c
  * st_wake_up is more or less just converting the IO. PIT0 replaces TIMSK1.
  * st_go_idle is the exact inverse of st_wake_up, and just as simple.
  * iterate_trapezoid_cycle_counter gets its return type changed to a full word
  * The big interrupt gets its name changed to run on PIT0
  * The reset interrupt changes from TCNT2 to PIT1
  * (1<<Z_BIT etc) just becomes Z_BIT because of our definition.
  * For the initialization, what rate do we run the timer at? It won't trigger interrupts at all.
  * config_step_timer became really really fucking easy, because of the 32 bit timer and lack of a prescaler.
  * My assumption is that config_step_timer takes immediate effect, given that it is messing with the prescaler.
  * Bug in Grbl! F_CPU above 2^32/60 (35.7MHz) cause an integer overflow in set_step_events_per_minute - solution is to add an explicit
      annotation that the 60 is a uint32_t.

### limits.h

  * Comparatively simple, although it did prompt me to make some IO configuration macro names more accurate. Otherwise good.

### planner.c

  * just change out names for direction bits

### nuts_bolts.c

  * Coverted everything to the vendor version of delay and delay_ms

### gcode.c
  * clean compile

### motion_control.c
  * enables/disables hard-limit interrupt, so I refactored that, and limits.c now has a function to set interrupt status.

### report.c
  * Just defined two macros to convert the calls to printPgmString to something along the lines of usb_serial_write(x,sizeof(x) - 1),
    where x is a string literal. Works, but could be more elegant.

### print.c
  * Replaced serial with usb serial.
  * Oddly, have to provide explicit declarations of the usb serial functions. This
    is annoying, and may hint that something is wrong.

### serial.c

  * only grbl-specific function is in the SERIAL_RX ISR - execute special characters for feed holds and resets as soon as possible.
    Unfortuntely, there is not an equivalent place in the usb serial stack for such an interrupt, and so dealing with this requires
    lots of low-level hacking of usb serial. Not for the initial porting run.
  * Thus, we move the code to flag special chars into the rest of the parser routine.

### settings.c

  * Actually, we want a slightly more full featured set of eeprom functions.
  * version is moved into the first field of the settings structure.
  * reset the version counter to 0 and take out all references to migrating from previous firmware versions!
  * changed eeprom to 1kb. Check number of coordinate axes and size of startup lines - checksums take up more room now.
