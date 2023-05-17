#include "eeprom.h"



namespace EEPROM {

    void init () {
        i2c_init(EEPROM_I2C_CHANNEL, 1000 * 1000); // init i2c at 100KHz - may be able to shift up to 1MHz or to 400KHz as EEPROM chip is running at 3.3V instead of 5V
        gpio_init(EEPROM_SDA_PIN);
        gpio_init(EEPROM_SCL_PIN);

        gpio_set_function(EEPROM_SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(EEPROM_SCL_PIN, GPIO_FUNC_I2C);
        
        gpio_pull_up(EEPROM_SDA_PIN);
        gpio_pull_up(EEPROM_SCL_PIN);

        // bus_scan();
        // test();
    }
    // void savePresetToEEPROM(uint16_t preset, const void* data, size_t length) {
    //     uint8_t     buffer[length + 2];
    //     uint16_t    address = 0;

    //     if (preset >= MAX_PRESETS || preset < 0) {
    //         printf("ERROR! Outside of Preset storage range!/n");
    //         return;
    //     }
    //     else {
    //         address = preset * PAGE_SIZE;
    //         printf("Saving Preset %d...\n", preset);
    //     }
        
    //     buffer[0] = (address >> 8);
    //     buffer[1] = (address & 0xFF);

    //     const uint8_t* dataBytes = reinterpret_cast<const uint8_t*>(data);
    //     for (size_t i = 0; i < length; i++) {
    //         buffer[i + 2] = dataBytes[i + 3];
    //     }

    //     i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, buffer, PAGE_SIZE + 2, false);

    //     printf("Preset saved to EEPROM at address: %d\n", address);
    // }
}