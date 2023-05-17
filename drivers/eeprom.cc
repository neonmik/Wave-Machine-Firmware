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
}