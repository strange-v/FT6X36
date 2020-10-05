# FT6X36 (FT6236/FT6336/FT6436L/FT6436)
This is an ESP-IDF component forked from ESP32 Arduino library [FT6X36](https://github.com/strange-v/FT6X36) for self-capacitive touch panel controllers produced by FocalTech Systems.

## ATTENTION!
Only basic functionality is currently implemented. Has been tested only on ESP32 using a 2.7" e-ink display (GDEW027W3-T).
This will work only as a compoment in the ESP-IDF framework. If you are looking for an arduino-esp32 version check the [original FT6X36 library](https://github.com/strange-v/FT6X36)

## Annotation
The [Adafruit_FT6206_Library](https://github.com/adafruit/Adafruit_FT6206_Library) was used as a base for the FT6X36 library. Many things were added/improved/rewritten based on a datasheet for the controller.

## References

The reading part of I2C was mostly stealed from [LVGL for ESP-IDF](https://github.com/lvgl/lv_port_esp32) but respecting the methods of the original library and done in a different way. Hats off to the great work of this library and FT6X36.
A second intention derived from this fork, is to make this library be less processor intensive, getting out the loop and reading only touch per interruption. Only when the INT pin goes from high to low (Falling edge, known as GPIO_INTR_NEGEDGE in ESP-IDF Framework)

[Video of the library working on ESP32 with GDEW027W3-T](https://twitter.com/martinfasani/status/1310294140329041921)

[Video of next step: To integrate this on an epaper component](https://twitter.com/martinfasani/status/1312877893001895943?s=21) and make it rotation and display aware.

[Example implemented on Cale-IDF our IDF component for epapers](https://github.com/martinberlin/cale-idf/wiki/Demo-catalog) in the cale-idf branch: feature/17-touch

## Donations accepted

If you like this component and it made your life easier please consider becoming a sponsor where you can donate as little as 2 u$ per month. Just click on:
❤ Sponsor  on the top right

♢ For cryptocurrency users is also possible to help this project transferring Ethereum:

0x68cEAB84F33776a7Fac977B2Bdc0D50933344086

We are thankful for the support and contributions so far!
