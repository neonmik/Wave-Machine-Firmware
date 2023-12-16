#include "eeprom.h"



namespace EEPROM {

    void init () {
        i2c_init(EEPROM_I2C_CHANNEL, 400 * 1000); // init i2c at 1MHz - runs lower as EEPROM chip is running at 3.3V instead of 5V
        gpio_init(EEPROM_SDA_PIN);
        gpio_init(EEPROM_SCL_PIN);

        gpio_set_function(EEPROM_SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(EEPROM_SCL_PIN, GPIO_FUNC_I2C);
        
        gpio_pull_up(EEPROM_SDA_PIN);
        gpio_pull_up(EEPROM_SCL_PIN);
    }

    void write (uint16_t address, uint8_t *src, uint16_t len) {

        uint16_t remaining_len = len;
        uint16_t current_address = address;

        while (remaining_len > 0) {
            uint16_t write_len = remaining_len > PAGE_SIZE ? PAGE_SIZE : remaining_len;

            // Format address
            uint8_t buffer[ADDRESS_SIZE + PAGE_SIZE] = {0};
            buffer[0] = ((current_address >> 8) & 0xFF);
            buffer[1] = (current_address & 0xFF);

            for (int i = 0; i < write_len; i++) {
                buffer[ADDRESS_SIZE + i] = src[i];
            }

            // printf("Writing to EEPROM address %d...\n", current_address);

            // Write the address and data to the EEPROM
            i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, buffer, ADDRESS_SIZE + write_len, false);
            sleep_ms(10); // wait for the EEPROM to finish writing

            // update variables
            remaining_len -= write_len;
            current_address += write_len;
            src += write_len;
        }
    }
    void read (uint16_t address, uint8_t *src, uint16_t len) {

        uint16_t remaining_len = len;
        uint16_t current_address = address;

        while (remaining_len > 0) {
            uint16_t read_len = remaining_len > PAGE_SIZE ? PAGE_SIZE : remaining_len;

            // Format address
            uint8_t addr[ADDRESS_SIZE];
            addr[0] = ((current_address >> 8) & 0xFF);
            addr[1] = (current_address & 0xFF);

            // printf("Reading from EEPROM address %d...\n", current_address);

            // Send address to the EEPROM
            i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, addr, ADDRESS_SIZE, false);
            sleep_ms(10); // wait for the EEPROM to finish writing

            // Read the data at the address
            i2c_read_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, src, read_len, false);
            sleep_ms(10); // wait for the EEPROM to finish reading

            // update variables
            remaining_len -= read_len;
            current_address += read_len;
            src += read_len;
        }
    }
    void erase (uint16_t address, uint16_t length) {
        
        uint8_t buffer[length]; 
        
        for (int i = 0; i < length; i++) {
            buffer[i] = 255;
        }
        write(address, buffer, length);
    }
    void eraseAll (void) {
        erase(0x0, MAX_ADDRESS+1);
    }


    uint8_t buffer[256] = {0};
    uint8_t rxdata[256] = {0};
    void testIntegrity (void) 
    {
        uint16_t size = 256;
        printf("Testing EEPROM integrity...\n");
        for (int i = 0; i < MAX_ADDRESS; i += size) {
            // Initialize the buffer
            for (int j = 0; j < size; ++j) {
                buffer[j] = j;
            }
            write(i, buffer, size);
            sleep_ms(10);
            read(i, rxdata, size);
            sleep_ms(10);
            for (int j = 0; j < size; ++j) {
                if (rxdata[j] != buffer[j]) {
                    printf("Integrity check failed at address %d\n", i + j);
                    printf("Expected: %d, Received: %d\n", buffer[j], rxdata[j]);
                }
            }
            printf(".");
        }
        printf("EEPROM integrity test complete!\n");
    }
}