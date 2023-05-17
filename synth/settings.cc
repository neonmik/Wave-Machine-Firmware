#include "settings.h"

namespace SETTINGS {
    PRESET Preset[MAX_PRESETS];
    CONTROL Control;

    void init () {
        EEPROM::init();

        _preset = _default_preset;
        load_preset(_preset);
    }

    void set_preset (uint8_t preset) {
        // save_preset();
        _preset = preset;
        load_preset(_preset);
        ARP::reset();
        _changed = true;
    }
    uint8_t get_preset () {
        return _preset;
    }
    void save_preset (uint8_t preset) {


        Preset[preset].Wave.shape = Control.get(0, 0);
        Preset[preset].Wave.vector = Control.get(0, 1);
        Preset[preset].Wave.octave = Control.get(0, 2);
        Preset[preset].Wave.pitch = Control.get(0, 3);

        Preset[preset].Envelope.attack = Control.get(1, 0);
        Preset[preset].Envelope.decay = Control.get(1, 1);
        Preset[preset].Envelope.sustain = Control.get(1, 2);
        Preset[preset].Envelope.release = Control.get(1, 3);

        Preset[preset].Modulation.state = Control.get_lfo();
        Preset[preset].Modulation.matrix = Control.get(2, 0);
        Preset[preset].Modulation.rate = Control.get(2, 1);
        Preset[preset].Modulation.depth = Control.get(2, 2);
        Preset[preset].Modulation.wave = Control.get(2, 3);

        Preset[preset].Arpeggiator.state = Control.get_arp();
        Preset[preset].Arpeggiator.hold = Control.get(3, 0);
        Preset[preset].Arpeggiator.divisisions = Control.get(3, 1);
        Preset[preset].Arpeggiator.range = Control.get(3, 2);
        Preset[preset].Arpeggiator.direction = Control.get(3, 3);
        
        // code here for saving preset infomation to EEPROM
    
        // uint8_t  preset_size = 36;
        // uint16_t preset_address = preset * preset_size;    
        // uint8_t buf[2];
        // buf[0] = (preset_address >> 8);
        // buf[1] = (preset_address & 0xFF);
        // i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, buf, 2, false);
        // sleep_ms(5); //can I remove?
        // i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, (uint8_t*)&Preset[preset], preset_size, false);
        // sleep_ms(5); //can I remove?

        uint8_t preset_size = 64;
        uint16_t preset_address = preset * preset_size;
        uint8_t buffer[preset_size+2];

        buffer[0] = (preset_address >> 8);
        buffer[1] = (preset_address & 0xFF);
        buffer[2] = (Preset[preset].Wave.shape >> 8) & 0xFF;
        buffer[3] = Preset[preset].Wave.shape & 0xFF;
        buffer[4] = (Preset[preset].Wave.vector >> 8) & 0xFF;
        buffer[5] = Preset[preset].Wave.vector & 0xFF;
        buffer[6] = (Preset[preset].Wave.octave >> 8) & 0xFF;
        buffer[7] = Preset[preset].Wave.octave & 0xFF;
        buffer[8] = (Preset[preset].Wave.pitch >> 8) & 0xFF;
        buffer[9] = Preset[preset].Wave.pitch & 0xFF;

        buffer[10] = (Preset[preset].Envelope.attack >> 8) & 0xFF;
        buffer[11] = Preset[preset].Envelope.attack & 0xFF;
        buffer[12] = (Preset[preset].Envelope.decay >> 8) & 0xFF;
        buffer[13] = Preset[preset].Envelope.decay & 0xFF;
        buffer[14] = (Preset[preset].Envelope.sustain >> 8) & 0xFF;
        buffer[15] = Preset[preset].Envelope.sustain & 0xFF;
        buffer[16] = (Preset[preset].Envelope.release >> 8) & 0xFF;
        buffer[17] = Preset[preset].Envelope.release & 0xFF;

        buffer[18] = Preset[preset].Modulation.state;
        buffer[19] = (Preset[preset].Modulation.matrix >> 8) & 0xFF;
        buffer[20] = Preset[preset].Modulation.matrix & 0xFF;
        buffer[21] = (Preset[preset].Modulation.rate >> 8) & 0xFF;
        buffer[22] = Preset[preset].Modulation.rate & 0xFF;
        buffer[23] = (Preset[preset].Modulation.depth >> 8) & 0xFF;
        buffer[24] = Preset[preset].Modulation.depth & 0xFF;
        buffer[25] = (Preset[preset].Modulation.wave >> 8) & 0xFF;
        buffer[26] = Preset[preset].Modulation.wave & 0xFF;

        buffer[27] = Preset[preset].Arpeggiator.state;
        buffer[28] = (Preset[preset].Arpeggiator.hold >> 8) & 0xFF;
        buffer[29] = Preset[preset].Arpeggiator.hold & 0xFF;
        buffer[30] = (Preset[preset].Arpeggiator.divisisions >> 8) & 0xFF;
        buffer[31] = Preset[preset].Arpeggiator.divisisions & 0xFF;
        buffer[32] = (Preset[preset].Arpeggiator.range >> 8) & 0xFF;
        buffer[33] = Preset[preset].Arpeggiator.range & 0xFF;
        buffer[34] = (Preset[preset].Arpeggiator.direction >> 8) & 0xFF;
        buffer[35] = Preset[preset].Arpeggiator.direction & 0xFF;

        i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, buffer, preset_size, false);
        sleep_ms(5); //can I remove?

        printf("Preset %d saved to EEPROM!\n", preset);
        printf("Memory Location: %d\n", preset_address);
        
        printf("Waveshape:  %d\n", Preset[preset].Wave.shape);
        printf("Vector:     %d\n", Preset[preset].Wave.vector);
        printf("Octave:     %d\n", Preset[preset].Wave.octave);
        printf("Pitch:      %d\n\n", Preset[preset].Wave.pitch);
        
        printf("Attack:     %d\n", Preset[preset].Envelope.attack);
        printf("Decay:      %d\n", Preset[preset].Envelope.decay);
        printf("Sustain:    %d\n", Preset[preset].Envelope.sustain);
        printf("Release:    %d\n\n", Preset[preset].Envelope.release);

        printf("LFO state:  %d\n", Preset[preset].Modulation.state);
        printf("matriix:    %d\n", Preset[preset].Modulation.matrix);
        printf("rate:       %d\n", Preset[preset].Modulation.rate);
        printf("depth:      %d\n", Preset[preset].Modulation.depth);
        printf("shape:      %d\n\n", Preset[preset].Modulation.wave);
        
        printf("ARP state:  %d\n", Preset[preset].Arpeggiator.state);
        printf("matrix:     %d\n", Preset[preset].Arpeggiator.hold);
        printf("rate:       %d\n", Preset[preset].Arpeggiator.divisisions);
        printf("depth:      %d\n", Preset[preset].Arpeggiator.range);
        printf("shape:      %d\n\n", Preset[preset].Arpeggiator.direction);

    }
    void load_preset (uint8_t preset) {
        // code here for loading preset infomation from EEPROM

        uint8_t  preset_size = 64;
        uint16_t preset_address = preset * preset_size;
        
        uint8_t buf[2];
        buf[0] = (preset_address >> 8);
        buf[1] = (preset_address & 0xFF);

        uint8_t buffer[sizeof(PRESET)];

        i2c_write_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, (uint8_t*)&buf, 2, false);
        sleep_ms(5); //can I remove?
        i2c_read_blocking(EEPROM_I2C_CHANNEL, EEPROM_I2C_ADDRESS, buffer, sizeof(PRESET), false);
        sleep_ms(5); //can I remove?

        Preset[preset].Wave.shape = (buffer[0] << 8) | buffer[1];
        Preset[preset].Wave.vector = (buffer[2] << 8) | buffer[3];
        Preset[preset].Wave.octave = (buffer[4] << 8) | buffer[5];
        Preset[preset].Wave.pitch = (buffer[6] << 8) | buffer[7];

        Preset[preset].Envelope.attack = (buffer[8] << 8) | buffer[9];
        Preset[preset].Envelope.decay = (buffer[10] << 8) | buffer[11];
        Preset[preset].Envelope.sustain = (buffer[12] << 8) | buffer[13];
        Preset[preset].Envelope.release = (buffer[14] << 8) | buffer[15];

        Preset[preset].Modulation.state = buffer[16];
        Preset[preset].Modulation.matrix = (buffer[17] << 8) | buffer[18];
        Preset[preset].Modulation.rate = (buffer[19] << 8) | buffer[20];
        Preset[preset].Modulation.depth = (buffer[21] << 8) | buffer[22];
        Preset[preset].Modulation.wave = (buffer[23] << 8) | buffer[24];

        Preset[preset].Arpeggiator.state = buffer[25];
        Preset[preset].Arpeggiator.hold = (buffer[26] << 8) | buffer[27];
        Preset[preset].Arpeggiator.divisisions = (buffer[28] << 8) | buffer[29];
        Preset[preset].Arpeggiator.range = (buffer[30] << 8) | buffer[31];
        Preset[preset].Arpeggiator.direction = (buffer[32] << 8) | buffer[33];
        
        printf("Preset %d read from EEPROM!\n", preset);
        printf("Memory Location: %d\n\n", preset_address);

        printf("Waveshape:  %d\n", Preset[preset].Wave.shape);
        printf("Vector:     %d\n", Preset[preset].Wave.vector);
        printf("Octave:     %d\n", Preset[preset].Wave.octave);
        printf("Pitch:      %d\n\n", Preset[preset].Wave.pitch);
        
        printf("Attack:     %d\n", Preset[preset].Envelope.attack);
        printf("Decay:      %d\n", Preset[preset].Envelope.decay);
        printf("Sustain:    %d\n", Preset[preset].Envelope.sustain);
        printf("Release:    %d\n\n", Preset[preset].Envelope.release);

        printf("LFO state:  %d\n", Preset[preset].Modulation.state);
        printf("matriix:    %d\n", Preset[preset].Modulation.matrix);
        printf("rate:       %d\n", Preset[preset].Modulation.rate);
        printf("depth:      %d\n", Preset[preset].Modulation.depth);
        printf("shape:      %d\n\n", Preset[preset].Modulation.wave);
        
        printf("ARP state:  %d\n", Preset[preset].Arpeggiator.state);
        printf("matrix:     %d\n", Preset[preset].Arpeggiator.hold);
        printf("rate:       %d\n", Preset[preset].Arpeggiator.divisisions);
        printf("depth:      %d\n", Preset[preset].Arpeggiator.range);
        printf("shape:      %d\n\n", Preset[preset].Arpeggiator.direction);

        Control.set(0, 0, Preset[preset].Wave.shape);
        Control.set(0, 1, Preset[preset].Wave.vector);
        Control.set(0, 2, Preset[preset].Wave.octave);
        Control.set(0, 3, Preset[preset].Wave.pitch);

        Control.set(1, 0, Preset[preset].Envelope.attack);
        Control.set(1, 1, Preset[preset].Envelope.decay);
        Control.set(1, 2, Preset[preset].Envelope.sustain);
        Control.set(1, 3, Preset[preset].Envelope.release);

        Control.set_lfo(Preset[preset].Modulation.state);
        Control.set(2, 0, Preset[preset].Modulation.matrix);
        Control.set(2, 1, Preset[preset].Modulation.rate);
        Control.set(2, 2, Preset[preset].Modulation.depth);
        Control.set(2, 3, Preset[preset].Modulation.wave);

        Control.set_arp(Preset[preset].Arpeggiator.state);
        Control.set(3, 0, Preset[preset].Arpeggiator.hold);
        Control.set(3, 1, Preset[preset].Arpeggiator.divisisions);
        Control.set(3, 2, Preset[preset].Arpeggiator.range);
        Control.set(3, 3, Preset[preset].Arpeggiator.direction);

        _changed = true;
    
    }

    void save () {
        save_preset(_preset);
    }

    void set_page (uint8_t page) {
        _changed = true;
        _page = page;
    }
    uint8_t get_page () {
        return _page;
    }

    void set_value (uint8_t page, uint8_t control, uint16_t input) {
        _changed = true;
        Control.set(page, control, input);
    }
    uint16_t get_value (uint8_t page, uint8_t control) {
        return Control.get(page, control);
    }
    
    
    void toggle_lfo () {
        _changed = true;
        Control.toggle_lfo();
    }
    bool get_lfo () {
        return Control.get_lfo();
    }

    void toggle_arp () {
        _changed = true;
        Control.toggle_arp();
    }
    bool get_arp () {
        return Control.get_arp();
    }
    
    void update () {
        if (_changed) {
            Control.update();
            
            _changed = false;
        }
    }
    
}                                                                             