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
        if (slot >= MAX_PRESETS) {
            printf("ERROR! Outside of Preset storage range!\n");
            return;
        }

        uint16_t address = slot * PAGE_SIZE;
        printf("Saving Preset %d...\n", slot);

        uint8_t buffer[ADDRESS_SIZE + PAGE_SIZE] = {0};
        buffer[0] = (address >> 8) & 0xFF;
        buffer[1] = address & 0xFF;


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
        buffer[30] = (preset.Arpeggiator.divisions >> 8) & 0xFF;
        buffer[31] = preset.Arpeggiator.divisions & 0xFF;
        buffer[32] = (preset.Arpeggiator.range >> 8) & 0xFF;
        buffer[33] = preset.Arpeggiator.range & 0xFF;
        buffer[34] = (preset.Arpeggiator.direction >> 8) & 0xFF;
        buffer[35] = preset.Arpeggiator.direction & 0xFF;
        buffer[36] = preset.Filter.state;
        buffer[37] = (preset.Filter.cutoff >> 8) & 0xFF;
        buffer[38] = preset.Filter.cutoff & 0xFF;
        buffer[39] = (preset.Filter.resonance >> 8) & 0xFF;
        buffer[40] = preset.Filter.resonance & 0xFF;
        buffer[41] = (preset.Filter.punch >> 8) & 0xFF;
        buffer[42] = preset.Filter.punch & 0xFF;
        buffer[43] = (preset.Filter.type >> 8) & 0xFF;
        buffer[44] = preset.Filter.type & 0xFF;
        // maybe add some sort of padding here? 
        // 0xAF = After
        // 0xBE = Before
        // buffer[36] = 0xAF;
        // buffer[37] = 0xAF;

        i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, buffer, ADDRESS_SIZE + PAGE_SIZE, false);

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
        printf("rate:       %d\n",      preset.Arpeggiator.divisions);
        printf("depth:      %d\n",      preset.Arpeggiator.range);
        printf("shape:      %d\n\n",    preset.Arpeggiator.direction);

        printf("FILT state: %d\n",      preset.Filter.state);
        printf("Cutoff:     %d\n",      preset.Filter.cutoff);
        printf("Resonance:  %d\n",      preset.Filter.resonance);
        printf("Punch:      %d\n",      preset.Filter.punch);
        printf("Type:       %d\n\n",    preset.Filter.type);
    }
    void loadPreset (uint8_t slot, PRESET &preset) {
        if (slot >= MAX_PRESETS) {
            printf("ERROR! Outside of Preset storage range!\n");
            return;
        }
        
        uint16_t preset_address = slot * PAGE_SIZE;

        uint8_t address_buffer[ADDRESS_SIZE];

        address_buffer[0] = ((preset_address >> 8) & 0xFF);
        address_buffer[1] = (preset_address & 0xFF);

        uint8_t preset_buffer[PAGE_SIZE] = {0};
        // uint8_t preset_buffer[PAGE_SIZE] = {0};

        i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, address_buffer, ADDRESS_SIZE, false);
        i2c_read_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, preset_buffer, PAGE_SIZE, false);

        preset.Wave.shape = (preset_buffer[0] << 8) | preset_buffer[1];
        preset.Wave.vector = (preset_buffer[2] << 8) | preset_buffer[3];
        preset.Wave.octave = (preset_buffer[4] << 8) | preset_buffer[5];
        preset.Wave.pitch = (preset_buffer[6] << 8) | preset_buffer[7];

        preset.Envelope.attack = (preset_buffer[8] << 8) | preset_buffer[9];
        preset.Envelope.decay = (preset_buffer[10] << 8) | preset_buffer[11];
        preset.Envelope.sustain = (preset_buffer[12] << 8) | preset_buffer[13];
        preset.Envelope.release = (preset_buffer[14] << 8) | preset_buffer[15];

        preset.Modulation.state = preset_buffer[16];
        preset.Modulation.matrix = (preset_buffer[17] << 8) | preset_buffer[18];
        preset.Modulation.rate = (preset_buffer[19] << 8) | preset_buffer[20];
        preset.Modulation.depth = (preset_buffer[21] << 8) | preset_buffer[22];
        preset.Modulation.wave = (preset_buffer[23] << 8) | preset_buffer[24];

        preset.Arpeggiator.state = preset_buffer[25];
        preset.Arpeggiator.hold = (preset_buffer[26] << 8) | preset_buffer[27];
        preset.Arpeggiator.divisions = (preset_buffer[28] << 8) | preset_buffer[29];
        preset.Arpeggiator.range = (preset_buffer[30] << 8) | preset_buffer[31];
        preset.Arpeggiator.direction = (preset_buffer[32] << 8) | preset_buffer[33];

        preset.Filter.state = preset_buffer[34];
        preset.Filter.cutoff = (preset_buffer[35] << 8) | preset_buffer[36];
        preset.Filter.resonance = (preset_buffer[37] << 8) | preset_buffer[38];
        preset.Filter.punch = (preset_buffer[39] << 8) | preset_buffer[40];
        preset.Filter.type = (preset_buffer[41] << 8) | preset_buffer[42];
        
        printf("Preset %d read from EEPROM!\n", slot);
        printf("Memory Location: %d\n\n", preset_address);

        printf("Waveshape:      %d\n",       preset.Wave.shape);
        printf("Vector:         %d\n",       preset.Wave.vector);
        printf("Octave:         %d\n",       preset.Wave.octave);
        printf("Pitch:          %d\n\n",     preset.Wave.pitch);
        
        printf("Attack:         %d\n",       preset.Envelope.attack);
        printf("Decay:          %d\n",       preset.Envelope.decay);
        printf("Sustain:        %d\n",       preset.Envelope.sustain);
        printf("Release:        %d\n\n",     preset.Envelope.release);

        printf("LFO state:      %d\n",       preset.Modulation.state);
        printf("Destination:    %d\n",       preset.Modulation.matrix);
        printf("Rate:           %d\n",       preset.Modulation.rate);
        printf("Depth:          %d\n",       preset.Modulation.depth);
        printf("Shape:          %d\n\n",     preset.Modulation.wave);
        
        printf("ARP state:      %d\n",       preset.Arpeggiator.state);
        printf("Hold:           %d\n",       preset.Arpeggiator.hold);
        printf("Division:       %d\n",       preset.Arpeggiator.divisions);
        printf("Range:          %d\n",       preset.Arpeggiator.range);
        printf("Direction:      %d\n\n",     preset.Arpeggiator.direction);

        printf("FILT state:     %d\n",       preset.Filter.state);
        printf("Cutoff:         %d\n",       preset.Filter.cutoff);
        printf("Resonance:      %d\n",       preset.Filter.resonance);
        printf("Punch:          %d\n",       preset.Filter.punch);
        printf("Type:           %d\n\n",     preset.Filter.type);
    }

    void restorePreset (uint8_t slot) {
        if (slot >= MAX_PRESETS) {
            printf("ERROR! Outside of Factory Preset storage range!\n");
            return;
        }
        // load Factory Preset from EEPROM into buffer
        uint16_t factory_preset_address = (slot * PAGE_SIZE) | (FACTORY_PRESET_ADDRESS);
        uint8_t factory_address_buffer[ADDRESS_SIZE];
        factory_address_buffer[0] = ((factory_preset_address >> 8) & 0xFF);
        factory_address_buffer[1] = (factory_preset_address & 0xFF);

        uint8_t transfer_buffer[PAGE_SIZE] = {0};
        printf("Loading Factory Preset %d...\n", slot);
        printf("EEPROM address: %d\n", factory_preset_address);
        
        // Commented while I check the code
        i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, factory_address_buffer, ADDRESS_SIZE, false);
        i2c_read_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, transfer_buffer, sizeof(PRESET), false);
        
        printf("Loaded into buffer!\n");
        // 

        // save Factory Preset into normal Preset range
        uint16_t output_address = slot * PAGE_SIZE;
        uint8_t output_buffer[ADDRESS_SIZE + PAGE_SIZE] = {0};
        output_buffer[0] = (output_address >> 8) & 0xFF;
        output_buffer[1] = output_address & 0xFF;
        for (int i = 0; i < PAGE_SIZE; i++) {
            output_buffer[i + ADDRESS_SIZE] = transfer_buffer[i];
        }

        printf("Saving over Preset %d\n", slot);
        printf("EEPROM address: %d\n", output_address);

        // Commented while I check the code
        i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, output_buffer, PAGE_SIZE + 2, false);
        printf("Successfully restored Preset %d to factory settings!\n", slot);
        //
    }
}