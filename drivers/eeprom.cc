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

    void savePreset(uint8_t slot, PRESET &preset) {
        uint8_t     buffer[sizeof(preset) + 2];
        uint16_t    address = 0;

        if (slot >= MAX_PRESETS || slot < 0) {
            printf("ERROR! Outside of Preset storage range!/n");
            return;
        }
        else {
            address = slot * PAGE_SIZE;
            printf("Saving Preset %d...\n", slot);
        }
        
        buffer[0] = (address >> 8);
        buffer[1] = (address & 0xFF);

        buffer[2] = (preset.Wave.shape >> 8) & 0xFF;
        buffer[3] = preset.Wave.shape & 0xFF;
        buffer[4] = (preset.Wave.vector >> 8) & 0xFF;
        buffer[5] = preset.Wave.vector & 0xFF;
        buffer[6] = (preset.Wave.octave >> 8) & 0xFF;
        buffer[7] = preset.Wave.octave & 0xFF;
        buffer[8] = (preset.Wave.pitch >> 8) & 0xFF;
        buffer[9] = preset.Wave.pitch & 0xFF;
        buffer[10] = (preset.Envelope.attack >> 8) & 0xFF;
        buffer[11] = preset.Envelope.attack & 0xFF;
        buffer[12] = (preset.Envelope.decay >> 8) & 0xFF;
        buffer[13] = preset.Envelope.decay & 0xFF;
        buffer[14] = (preset.Envelope.sustain >> 8) & 0xFF;
        buffer[15] = preset.Envelope.sustain & 0xFF;
        buffer[16] = (preset.Envelope.release >> 8) & 0xFF;
        buffer[17] = preset.Envelope.release & 0xFF;
        buffer[18] = preset.Modulation.state;
        buffer[19] = (preset.Modulation.matrix >> 8) & 0xFF;
        buffer[20] = preset.Modulation.matrix & 0xFF;
        buffer[21] = (preset.Modulation.rate >> 8) & 0xFF;
        buffer[22] = preset.Modulation.rate & 0xFF;
        buffer[23] = (preset.Modulation.depth >> 8) & 0xFF;
        buffer[24] = preset.Modulation.depth & 0xFF;
        buffer[25] = (preset.Modulation.wave >> 8) & 0xFF;
        buffer[26] = preset.Modulation.wave & 0xFF;
        buffer[27] = preset.Arpeggiator.state;
        buffer[28] = (preset.Arpeggiator.hold >> 8) & 0xFF;
        buffer[29] = preset.Arpeggiator.hold & 0xFF;
        buffer[30] = (preset.Arpeggiator.divisisions >> 8) & 0xFF;
        buffer[31] = preset.Arpeggiator.divisisions & 0xFF;
        buffer[32] = (preset.Arpeggiator.range >> 8) & 0xFF;
        buffer[33] = preset.Arpeggiator.range & 0xFF;
        buffer[34] = (preset.Arpeggiator.direction >> 8) & 0xFF;
        buffer[35] = preset.Arpeggiator.direction & 0xFF;

        i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, buffer, PAGE_SIZE + 2, false);

        printf("Preset saved to EEPROM at address: %d\n", address);

        printf("Waveshape:  %d\n",      preset.Wave.shape);
        printf("Vector:     %d\n",      preset.Wave.vector);
        printf("Octave:     %d\n",      preset.Wave.octave);
        printf("Pitch:      %d\n\n",    preset.Wave.pitch);
        
        printf("Attack:     %d\n",      preset.Envelope.attack);
        printf("Decay:      %d\n",      preset.Envelope.decay);
        printf("Sustain:    %d\n",      preset.Envelope.sustain);
        printf("Release:    %d\n\n",    preset.Envelope.release);

        printf("LFO state:  %d\n",      preset.Modulation.state);
        printf("matriix:    %d\n",      preset.Modulation.matrix);
        printf("rate:       %d\n",      preset.Modulation.rate);
        printf("depth:      %d\n",      preset.Modulation.depth);
        printf("shape:      %d\n\n",    preset.Modulation.wave);
        
        printf("ARP state:  %d\n",      preset.Arpeggiator.state);
        printf("matrix:     %d\n",      preset.Arpeggiator.hold);
        printf("rate:       %d\n",      preset.Arpeggiator.divisisions);
        printf("depth:      %d\n",      preset.Arpeggiator.range);
        printf("shape:      %d\n\n",    preset.Arpeggiator.direction);
    }

    void loadPreset (uint8_t slot, PRESET &preset) {
        uint16_t preset_address = slot * PAGE_SIZE;
        
        uint8_t buf[2];
        buf[0] = (preset_address >> 8);
        buf[1] = (preset_address & 0xFF);

        uint8_t buffer[sizeof(PRESET)];

        i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, (uint8_t*)&buf, 2, false);
        sleep_ms(5); //can I remove?
        i2c_read_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, buffer, sizeof(PRESET), false);
        sleep_ms(5); //can I remove?

        preset.Wave.shape = (buffer[0] << 8) | buffer[1];
        preset.Wave.vector = (buffer[2] << 8) | buffer[3];
        preset.Wave.octave = (buffer[4] << 8) | buffer[5];
        preset.Wave.pitch = (buffer[6] << 8) | buffer[7];

        preset.Envelope.attack = (buffer[8] << 8) | buffer[9];
        preset.Envelope.decay = (buffer[10] << 8) | buffer[11];
        preset.Envelope.sustain = (buffer[12] << 8) | buffer[13];
        preset.Envelope.release = (buffer[14] << 8) | buffer[15];

        preset.Modulation.state = buffer[16];
        preset.Modulation.matrix = (buffer[17] << 8) | buffer[18];
        preset.Modulation.rate = (buffer[19] << 8) | buffer[20];
        preset.Modulation.depth = (buffer[21] << 8) | buffer[22];
        preset.Modulation.wave = (buffer[23] << 8) | buffer[24];

        preset.Arpeggiator.state = buffer[25];
        preset.Arpeggiator.hold = (buffer[26] << 8) | buffer[27];
        preset.Arpeggiator.divisisions = (buffer[28] << 8) | buffer[29];
        preset.Arpeggiator.range = (buffer[30] << 8) | buffer[31];
        preset.Arpeggiator.direction = (buffer[32] << 8) | buffer[33];
        
        printf("Preset %d read from EEPROM!\n", preset);
        printf("Memory Location: %d\n\n", preset_address);

        printf("Waveshape:  %d\n", preset.Wave.shape);
        printf("Vector:     %d\n", preset.Wave.vector);
        printf("Octave:     %d\n", preset.Wave.octave);
        printf("Pitch:      %d\n\n", preset.Wave.pitch);
        
        printf("Attack:     %d\n", preset.Envelope.attack);
        printf("Decay:      %d\n", preset.Envelope.decay);
        printf("Sustain:    %d\n", preset.Envelope.sustain);
        printf("Release:    %d\n\n", preset.Envelope.release);

        printf("LFO state:  %d\n", preset.Modulation.state);
        printf("matriix:    %d\n", preset.Modulation.matrix);
        printf("rate:       %d\n", preset.Modulation.rate);
        printf("depth:      %d\n", preset.Modulation.depth);
        printf("shape:      %d\n\n", preset.Modulation.wave);
        
        printf("ARP state:  %d\n", preset.Arpeggiator.state);
        printf("matrix:     %d\n", preset.Arpeggiator.hold);
        printf("rate:       %d\n", preset.Arpeggiator.divisisions);
        printf("depth:      %d\n", preset.Arpeggiator.range);
        printf("shape:      %d\n\n", preset.Arpeggiator.direction);
    }
}