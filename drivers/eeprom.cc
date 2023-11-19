#include "eeprom.h"



namespace EEPROM {

    void init () {
        i2c_init(EEPROM_I2C_CHANNEL, 1000 * 1000); // init i2c at 1MHz - runs higher as EEPROM chip is running at 3.3V instead of 5V
        gpio_init(EEPROM_SDA_PIN);
        gpio_init(EEPROM_SCL_PIN);

        gpio_set_function(EEPROM_SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(EEPROM_SCL_PIN, GPIO_FUNC_I2C);
        
        gpio_pull_up(EEPROM_SDA_PIN);
        gpio_pull_up(EEPROM_SCL_PIN);

        // Once the system settings code is outlined, it should be called here on startup.
        // loadSystemSettings();
    }

    void saveSystemSettings (void) {
        // Add code here for saving system settings
    }
    void loadSystemSettings (void) {
        // Add code here for loading system settings
    }

    void savePreset(uint8_t slot, PRESET &preset) {
        if (!checkSlot(slot)) {
            return;
        }

        uint16_t address = slot * PRESET_SIZE;

        writePreset(address, preset);
    }
    void loadPreset (uint8_t slot, PRESET &preset) {
        if (!checkSlot(slot)) {
            return;
        }
        
        uint16_t address = slot * PRESET_SIZE;
        
        readPreset(address, preset);

        // printPresetData(preset); 
    }
    void transferPreset (uint16_t from, uint16_t to) {

        printf("Starting Preset transfer...\n\n");

        PRESET buffer;
        
        // Read the data from the EEPROM
        readPreset(from, buffer);
        writePreset(to, buffer);

        printf("Preset transfer complete!\n\n");
    }
    void clearPreset (uint8_t slot) {
        if (!checkSlot(slot)) {
            return;
        }

        uint16_t address = slot * PRESET_SIZE;

        clearAddress(address, PRESET_SIZE);
    }
    

    void restoreFactoryPreset (uint8_t slot) {
        if (!checkSlot(slot)) {
            return;
        }
        // load Factory Preset from EEPROM into buffer
        uint16_t factory_preset_address = (slot * PRESET_SIZE) | (FACTORY_PRESET_ADDRESS);
        uint16_t output_address = slot * PRESET_SIZE;
        
        transferPreset(factory_preset_address, output_address);
    }
    
    void writeFactoryPreset (uint8_t slot) {
        if (!checkSlot(slot)) {
            return;
        }
        uint16_t preset_address = slot * PRESET_SIZE;
        uint16_t factory_preset_address = (slot * PRESET_SIZE) | (FACTORY_PRESET_ADDRESS);

        PRESET temp;

        transferPreset(preset_address, factory_preset_address);
    }

    void clearFreeMemory (void) {
        for (int i = 0; i < 6; i++) {
            uint16_t address = FREE_ADDRESS + (PRESET_SIZE * i);
            clearAddress(address, PRESET_SIZE);
        }
    }
}