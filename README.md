DS18B20 OneWire Mbed OS Example
===============================

Example program for using DS18B20 with [onewire-mbed-os](https://github.com/alwint3r/onewire-mbed-os) library.

## Import

Import this program to your workspace using mbed-cli using the following command:

```sh
mbed import https://github.com/alwint3r/ds18b20-example-onewire-mbed-os
```

## Compile and Flash the Program

Specify the target and toolchain to be used for compiling this program. For example, I'm going to use GCC as the toolchain and STM32 NUCLEO-F446RE as the target board/MCU. Here's the following command for that

```sh
mbed compile -t GCC_ARM -m NUCLEO_F446RE -f
```

