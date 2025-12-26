# What is this project?
This project is an implementation of the zmk uart split protocol that doesn't use zephyr,
the goal of this project is to allow the use of highly efficent and cheap microcontrollers
which can increase battery life when using a wired split wireless keyboard
instead of the 32 bit mcus that are required for zephyr to operate.
it also supports more efficent matrix scanning code and different debounce methods.
# How do i use it?
to use it you first have to configure it with the appropriate configuration for your keyboard in the file `main/config.h`
then you can build it using `pio run -e [mcu]`
and finaly you need to flash the built firmware using the appropriate method for your mcu.
the Currently supported mcus are
* msp430g2x53
* nano (atmega328p/arduino nano)
# Can I use it in my commercial product?
this project is licensed under the GPLv3 license, which means that in order to use it in your product you must provide
any modifications you have made to this code base to your consumers as implied by the gpl license.
