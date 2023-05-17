#pragma once

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h" // ??? Needed/What is it?

#include "../synth/settings.h"
#include "../synth/preset.h"

#include "hardware/i2c.h"

#define EEPROM_SDA_PIN          2
#define EEPROM_SCL_PIN          3

#define EEPROM_I2C_CHANNEL      i2c1
#define EEPROM_I2C_ADDRESS      0x50

#define PAGE_SIZE               64
#define MAX_ADDRESS             0x7FFF




namespace EEPROM {
    namespace {

        void write (uint16_t dataAddress, uint8_t dataVal) {
            uint8_t buf[3];

            buf[0] = (dataAddress >> 8);
            buf[1] = (dataAddress & 0xFF);
            buf[2] = dataVal;

            i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, buf, 3, false);
            sleep_ms(5); //can I remove?
        }
        uint8_t read (uint16_t dataAddress) {
            uint8_t buf[2];
            buf[0] = (dataAddress >> 8);
            buf[1] = (dataAddress & 0xFF);
            
            uint8_t readData = 0xFF;


            i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, buf, 2, false);
            sleep_ms(5); //can I remove?
            i2c_read_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, &readData, 2, false);
            sleep_ms(5); //can I remove?
            return readData;
        }
        void clear (uint16_t dataAddress) {
            write(dataAddress,0);
        }
        

        // EEPROM test functions
        void test (void) {
            uint16_t address = 0;
            uint16_t empty = 0;
            uint16_t full = 0;
            
            // read value first, see what the memory holds, and then print it
            empty = read(address);
            printf("Current Memory: %d \n", empty);
            
            // clear memory address
            clear(address);
            
            // read it and print it again to check its clear
            empty = read(address);
            printf("Empty Memory: %d \n", empty);
            
            // write a new number in the address
            write(address,255);
            
            // read (hopefully) new number and print it
            full = read(address);
            printf("New Memory: %d \n", full);
        }
        // I2C test functions
        bool reserved_addr(uint8_t addr) {
            // I2C reserves some addresses for special purposes. We exclude these from the scan.
            // These are any addresses of the form 000 0xxx or 111 1xxx
            return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
        }
        void bus_scan () {
            // performs a scan of the active I2C devices on the bus
            printf("\nI2C Bus Scan\n");
            printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

            for (int addr = 0; addr < (1 << 7); ++addr) {
                if (addr % 16 == 0) {
                    printf("%02x ", addr);
                }

                // Perform a 1-byte dummy read from the probe address. If a slave
                // acknowledges this address, the function returns the number of bytes
                // transferred. If the address byte is ignored, the function returns
                // -1.

                // Skip over any reserved addresses.
                int ret;
                uint8_t rxdata;
                if (reserved_addr(addr))
                    ret = PICO_ERROR_GENERIC;
                else
                    ret = i2c_read_blocking(EEPROM_I2C_CHANNEL, addr, &rxdata, 1, false);

                printf(ret < 0 ? "." : "@");
                printf(addr % 16 == 15 ? "\n" : "  ");
            }
            printf("Done.\n");
        }
    }

    void init (void);
    void savePreset(uint8_t slot, PRESET &preset);
    void loadPreset (uint8_t slot, PRESET &preset);

}