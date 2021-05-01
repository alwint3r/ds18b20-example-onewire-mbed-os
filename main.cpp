#include <OneWire.h>
#include <mbed.h>

// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// Based on version by PJRC
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

// Change the definition to your pin!
#define DS18B20_DATA_PIN D7


OneWire ds(DS18B20_DATA_PIN); // on pin 10 (a 4.7K resistor is necessary)

void setup(void)
{
}

void loop(void)
{
    uint8_t i;
    uint8_t present = 0;
    uint8_t type_s;
    uint8_t data[12];
    uint8_t addr[8];
    float celsius, fahrenheit;

    if (!ds.search(addr))
    {
        printf("No more addresses.\r\n\r\n");
        ds.reset_search();
        thread_sleep_for(250);
        return;
    }

    printf("ROM = ");
    for (i = 0; i < 8; i++)
    {
        printf(" %x", addr[i]);
    }

    if (OneWire::crc8(addr, 7) != addr[7])
    {
        printf("CRC is not valid!\r\n\r\n");
        return;
    }

    // the first ROM byte indicates which chip
    switch (addr[0])
    {
    case 0x10:
        printf("  Chip = DS18S20\r\n"); // or old DS1820
        type_s = 1;
        break;
    case 0x28:
        printf("  Chip = DS18B20\r\n");
        type_s = 0;
        break;
    case 0x22:
        printf("  Chip = DS1822\r\n");
        type_s = 0;
        break;
    default:
        printf("Device is not a DS18x20 family device.\r\n");
        return;
    }

    ds.reset();
    ds.select(addr);
    ds.write(0x44, 1); // start conversion, with parasite power on at the end

    thread_sleep_for(1000); // maybe 750ms is enough, maybe not
    // we might do a ds.depower() here, but the reset will take care of it.

    present = ds.reset();
    ds.select(addr);
    ds.write(0xBE); // Read Scratchpad

    printf("  Data = %x \r\n\r\n", present);
    for (i = 0; i < 9; i++)
    { // we need 9 bytes
        data[i] = ds.read();
        printf(" %x", data[i]);
    }
    printf("\r\n");
    printf(" CRC= %x \r\n\r\n", OneWire::crc8(data, 8));

    // Convert the data to actual temperature
    // because the result is a 16 bit signed integer, it should
    // be stored to an "int16_t" type, which is always 16 bits
    // even when compiled on a 32 bit processor.
    int16_t raw = (data[1] << 8) | data[0];
    if (type_s)
    {
        raw = raw << 3; // 9 bit resolution default
        if (data[7] == 0x10)
        {
            // "count remain" gives full 12 bit resolution
            raw = (raw & 0xFFF0) + 12 - data[6];
        }
    }
    else
    {
        uint8_t cfg = (data[4] & 0x60);
        // at lower res, the low bits are undefined, so let's zero them
        if (cfg == 0x00)
            raw = raw & ~7; // 9 bit resolution, 93.75 ms
        else if (cfg == 0x20)
            raw = raw & ~3; // 10 bit res, 187.5 ms
        else if (cfg == 0x40)
            raw = raw & ~1; // 11 bit res, 375 ms
                            //// default is 12 bit resolution, 750 ms conversion time
    }
    celsius = (float)raw / 16.0;
    fahrenheit = celsius * 1.8 + 32.0;
    printf("  Temperature: \r\n   %f Celsius \r\n   %f Fahrenheit \r\n\r\n", celsius, fahrenheit);
}

int main()
{
    setup();

    while (true)
    {
        loop();
    }
}