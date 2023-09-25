#pragma once

#include <stdio.h>

#include "pico/stdlib.h"
#include <string.h> // for memcpy

// #include "../synth/controls.h"
#include "../synth/preset.h"

#include "../debug.h"
#include "../config.h"

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
        // Prints
        void printPresetData (PRESET &preset) {

            printf("\n----------------------------------------------------\n");
            printf("                   OSCILLATOR\n");
            printf("----------------------------------------------------\n\n");

            printf("Waveshape:      %04d    |        ",      preset.Wave.shape);
                printf("Attack:         %04d\n",      preset.Envelope.attack);
            printf("Vector:         %04d    |        ",      preset.Wave.vector);
                printf("Decay:          %04d\n",      preset.Envelope.decay);
            printf("Octave:         %04d    |        ",      preset.Wave.octave);
                printf("Sustain:        %04d\n",      preset.Envelope.sustain);
            printf("Pitch:          %04d    |        ",    preset.Wave.pitch);
                printf("Release:        %04d\n\n",    preset.Envelope.release);
            

            printf("----------------------------------------------------\n");
            printf("                     FILTER\n");
            printf("----------------------------------------------------\n\n");

            printf("State:          ");
            if (preset.Filter.state == true) { printf("Enabled\n"); }
            else { printf("Disabled\n"); }

            printf("Cutoff:         %04d    |        ",      preset.Filter.cutoff);
                printf("Attack:         %04d\n",      preset.Filter.attack);
            printf("Resonance:      %04d    |        ",      preset.Filter.resonance);
                printf("Decay:          %04d\n",      preset.Filter.decay);
            printf("Punch:          %04d    |        ",      preset.Filter.punch);
                printf("Sustain:        %04d\n",      preset.Filter.sustain);
            printf("Type:           %04d    |        ",    preset.Filter.type);
                printf("Release:        %04d\n\n",    preset.Filter.release);


            printf("----------------------------------------------------\n");
            printf("                      LFO\n");
            printf("----------------------------------------------------\n\n");

            printf("State:          ");
            if (preset.Modulation.state == true) { printf("Enabled\n"); }
            else { printf("Disabled\n"); }

            printf("Destination:    %04d\n",      preset.Modulation.matrix);
            printf("Rate:           %04d\n",      preset.Modulation.rate);
            printf("Depth:          %04d\n",      preset.Modulation.depth);
            printf("Shape:          %04d\n\n",    preset.Modulation.wave);
            

            printf("----------------------------------------------------\n");
            printf("                      ARP\n");
            printf("----------------------------------------------------\n\n");

            printf("State:          ");
            if (preset.Arpeggiator.state == true) { printf("Enabled\n"); }
            else { printf("Disabled\n"); }

            printf("Gate:           %04d    |        ",      preset.Arpeggiator.gate);
                printf("Rest:           %04d\n",      preset.Arpeggiator.rest);
            printf("Rate/Division:  %04d    |        ",      preset.Arpeggiator.divisions);
                printf("BPM:            %04d\n",      preset.Arpeggiator.bpm);
            printf("Depth:          %04d    |        ",      preset.Arpeggiator.range);
                printf("Filter Mode:    %04d\n",      preset.Arpeggiator.fMode);
            printf("Shape:          %04d    |        ",    preset.Arpeggiator.direction);
                printf("Octave Mode:    %04d\n\n",    preset.Arpeggiator.octMode);


            printf("----------------------------------------------------\n");
            printf("                      FX\n");
            printf("----------------------------------------------------\n\n");

            printf("Gain:           %04d\n\n",      preset.Effects.gain);
        }

        // Checks
        bool checkLength (uint8_t len) {
            if (len > PRESET_SIZE) {
                DEBUG::warning("EEPROM length longer than 2 Pages (128 bytes) which is not supported");
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
        
        // Functions
        void write (uint16_t address, uint8_t *src, uint8_t len) {
            if (!checkLength(len)) {
                return; // Don't proceed if the length is invalid.
            }

            uint16_t remaining_len = len;
            uint16_t current_address = address;

            while (remaining_len > 0) {
                uint8_t write_len = remaining_len > PAGE_SIZE ? PAGE_SIZE : remaining_len;

                // Format address
                uint8_t buffer[ADDRESS_SIZE + PAGE_SIZE];
                buffer[0] = ((current_address >> 8) & 0xFF);
                buffer[1] = (current_address & 0xFF);

                for (int i = 0; i < write_len; i++) {
                    buffer[ADDRESS_SIZE + i] = src[i];
                }

                // printf("Writing to EEPROM address %d...\n", current_address);

                // Write the address and data to the EEPROM
                i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, buffer, ADDRESS_SIZE + write_len, false);

                // update variables
                remaining_len -= write_len;
                current_address += write_len;
                src += write_len;
            }
        }
        void read (uint16_t address, uint8_t *src, uint8_t len) {
            if (!checkLength(len)) {
                return; // Abort if the length is invalid
            }

            uint16_t remaining_len = len;
            uint16_t current_address = address;

            while (remaining_len > 0) {
                uint8_t read_len = remaining_len > PAGE_SIZE ? PAGE_SIZE : remaining_len;

                // Format address
                uint8_t addr[ADDRESS_SIZE];
                addr[0] = ((current_address >> 8) & 0xFF);
                addr[1] = (current_address & 0xFF);

                // printf("Reading from EEPROM address %d...\n", current_address);

                // Send address to the EEPROM
                i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, addr, ADDRESS_SIZE, false);

                // Read the data at the address
                i2c_read_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, src, read_len, false);

                // update variables
                remaining_len -= read_len;
                current_address += read_len;
                src += read_len;
            }
        }
        
        void writePreset (uint16_t address, PRESET &preset) {
            // Create a buffer to hold the data - we use a preset size (2 pages/128 bytes) to ensure data is spaced properly
            // We init with 0 to make sure theres no data pulled from elsewhere.
            uint8_t buffer[PRESET_SIZE] = {0};

            // Use memcpy to copy the data from preset to buffer
            memcpy(buffer, &preset, sizeof(PRESET));

            // Write the data in buffer to EEPROM
            EEPROM::write(address, buffer, PRESET_SIZE);
        }
        void readPreset (uint16_t address, PRESET &preset) {
            // Create a buffer to hold the data - currently using sizeof to make sure we don't overflow the actual struct allocation.
            uint8_t buffer[PRESET_SIZE];
            
            // Read the data from the EEPROM address
            EEPROM::read(address, buffer, PRESET_SIZE);

            memcpy(&preset, buffer, sizeof(PRESET));
        }
        
        void writeSettings (uint16_t address) {

        }
        void readSettings (uint16_t address) {

        }
        
        void clearAddress (uint16_t address, uint8_t length) {
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
    void savePreset             (uint8_t slot, PRESET &preset);
    void loadPreset             (uint8_t slot, PRESET &preset);
    void clearPreset            (uint8_t slot);

    void restoreFactoryPreset   (uint8_t slot);
    void writeFactoryPreset     (uint8_t slot);

    void transferPreset         (uint16_t from, uint16_t to);

    void clearFreeMemory (void);
}