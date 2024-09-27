#pragma once

#include "../config.h"

#include "hardware/i2c.h"





namespace EEPROM {
    
    void init (void);

    void write (uint16_t address, uint8_t *src, uint16_t len);
    void read (uint16_t address, uint8_t *src, uint16_t len);
    void erase (uint16_t address, uint16_t len);
    void eraseAll (void);

    namespace {
        // Checks
        bool checkPresetLength (uint8_t len) {
            if (len > PRESET_SIZE) {
                DEBUG::warning("EEPROM length longer than 2 Pages (128 bytes) which is currently not supported");
                return false;
            } else if (len == 0) {
                DEBUG::warning("EEPROM length of 0... No data will be sent");
                return false;
            } else {
                return true;
            }
        }

        bool checkSlot (uint8_t slot) {
            if (slot >= MAX_PRESETS) {
                DEBUG::error("Outside of Preset storage range!");
                return false;
            } else {
                return true;
            }
        }
        
        void clearAddress (uint16_t address, uint16_t length) {
            // clear the buffer
            uint8_t buffer[length]; 
            for (int i = 0; i < length; i++) {
                buffer[i] = 255;
            }
            // write both pages
            write(address, buffer, length);
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
            clearAddress(address, 1);
            
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
        void testIntegrity (void) {
            uint8_t buffer = 0;
            for (int i = 0; i < MAX_ADDRESS; i++) {
                write(i, &buffer, 1);
                buffer++; // should overflow every 256 writes
                uint8_t rxdata;
                read(i, &rxdata, 1);
                if (rxdata != buffer) {
                    printf("Integrity check failed at address %d\n", i);
                    printf("Expected: %d, Received: %d\n", buffer, rxdata);
                    return;
                }
            }

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

    void testIntegrity (void);
}