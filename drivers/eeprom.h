#pragma once

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h" // ??? Needed/What is it?

#include "../synth/controls.h"
#include "../synth/preset.h"

#include "hardware/i2c.h"

constexpr   uint8_t         EEPROM_SDA_PIN          =       2;
constexpr   uint8_t         EEPROM_SCL_PIN          =       3;

constexpr   i2c_inst_t*     EEPROM_I2C_CHANNEL      =       i2c1;
constexpr   uint8_t         EEPROM_I2C_ADDRESS      =       0x50;

constexpr   uint8_t         ADDRESS_SIZE            =       2;                  // The size of the EEPROM address
constexpr   uint8_t         PAGE_SIZE               =       64;                 // The size of a Page in the EEPROM (64 bytes)
constexpr   uint8_t         PRESET_SIZE             =       (PAGE_SIZE * 2);    // The size of a Preset (currently 64 bytes, but may become more in future, hence 128 bytes per preset)

constexpr   uint16_t        PRESET_ADDRESS          =       0x0;                // Pages 0    - 127 are for Presets
constexpr   uint16_t        FACTORY_PRESET_ADDRESS  =       0x2000;             // Pages 128  - 255 are for Factory Presets
constexpr   uint16_t        FREE_ADDRESS            =       0x4000;             // Pages 256  - 4091 are empty for now
constexpr   uint16_t        SYSTEM_SETTINGS_ADDRESS =       0x7EFF;             // Pages 4092 - 4096 are for System Settings
constexpr   uint16_t        MAX_ADDRESS             =       0x7FFF;             // The last readable address in the EEPROM




namespace EEPROM {
    namespace {
        void checkLength (uint8_t len) {
            if (len > PAGE_SIZE) {
                printf ("WARNING! EEPROM length longer than 1 Page (64 bytes) which is currently not supported\n");
                // could either force return here, or split message somehow?
            } else if (len == 0) {
                printf ("WARNING! EEPROM length of 0... No data will be sent\n");
                return;
            }
        }
        
        void write (uint16_t addr, uint8_t *src, uint8_t len) {

            checkLength(len);

            // Format address - is this needed?
            uint8_t buffer[ADDRESS_SIZE + len];
            buffer[0] = ((addr >> 8) & 0xFF);
            buffer[1] = (addr & 0xFF);

            for (int i = 0; i < len; i++) {
                buffer[ADDRESS_SIZE + i] = src[i];
            }

            printf("Writing to EEPROM address %d...\n", addr);

            // Write the address and data to the EEPROM
            i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, buffer, ADDRESS_SIZE + len, false);
        }
        void read (uint16_t addr, uint8_t *src, uint8_t len) {
            
            checkLength(len);

            // Format address - is this needed?
            uint8_t address[ADDRESS_SIZE];
            address[0] = ((addr >> 8) & 0xFF);
            address[1] = (addr & 0xFF);

            printf("Reading from EEPROM address %d...\n", addr);

            // Send address to the EEPROM
            i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, address, ADDRESS_SIZE, false);
            // Read the data at the address
            i2c_read_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, src, len, false);
        }

        void clear (uint16_t address) {
            write(address, 0, 1);
        }
        // EEPROM test functions
        void test (void) {
            // currently commented as was using old API.

            uint16_t address = FREE_ADDRESS;
            uint8_t empty = 0;
            uint8_t temp = 0;
            uint8_t full = 0;
            
            // read value first, see what the memory holds, and then print it
            read(address, &temp, sizeof(temp));
            printf("Current Memory: %d \n", temp);
            
            // clear memory address
            clear(address);
            
            // read it and print it again to check its clear
            read(address, &empty, sizeof(empty));
            printf("Empty Memory: %d \n", empty);
            
            // write a new number in the address
            temp++;

            write(address, &temp, sizeof(temp));
            
            // read (hopefully) new number and print it
            read(address, &full, sizeof(full));
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

    void Init (void);
    void savePreset(uint8_t slot, PRESET &preset);
    void loadPreset (uint8_t slot, PRESET &preset);
    void clearPreset (uint8_t slot);

    void restoreFactoryPreset (uint8_t slot);
    void writeFactoryPreset (uint8_t slot);

    void transferPreset (uint16_t fromAddress, uint16_t toAddress);

    void test_save (PRESET &preset);
}