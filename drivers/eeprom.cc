#include "eeprom.h"



namespace EEPROM {

    void Init () {
        i2c_init(EEPROM_I2C_CHANNEL, 1000 * 1000); // Init i2c at 1MHz - runs higher as EEPROM chip is running at 3.3V instead of 5V
        gpio_init(EEPROM_SDA_PIN);
        gpio_init(EEPROM_SCL_PIN);

        gpio_set_function(EEPROM_SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(EEPROM_SCL_PIN, GPIO_FUNC_I2C);
        
        gpio_pull_up(EEPROM_SDA_PIN);
        gpio_pull_up(EEPROM_SCL_PIN);

        // bus_scan();
        // test();

        // Once the system settings code is outlined, it should be called here on startup.
        // loadSystemSettings();
    }
    void saveSystemSettings (void) {
        // Add code here for saving system settings
    }
    void loadSystemSettings (void) {
        // Add code here for loading system settings
    }

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

        printf("State:     ");
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

        printf("State:      ");
        if (preset.Modulation.state == true) { printf("Enabled\n"); }
        else { printf("Disabled\n"); }

        printf("Destination:    %04d\n",      preset.Modulation.matrix);
        printf("Rate:           %04d\n",      preset.Modulation.rate);
        printf("Depth:          %04d\n",      preset.Modulation.depth);
        printf("Shape:          %04d\n\n",    preset.Modulation.wave);
        

        printf("----------------------------------------------------\n");
        printf("                      ARP\n");
        printf("----------------------------------------------------\n\n");

        printf("State:      ");
        if (preset.Arpeggiator.state == true) { printf("Enabled\n"); }
        else { printf("Disabled\n"); }

        printf("Gate:           %04d\n",      preset.Arpeggiator.gate);
        printf("Rate/Division:  %04d\n",      preset.Arpeggiator.divisions);
        printf("Depth:          %04d\n",      preset.Arpeggiator.range);
        printf("Shape:          %04d\n\n",    preset.Arpeggiator.direction);

        printf("Rest:           %04d\n",      preset.Arpeggiator.rest);
        printf("BPM:            %04d\n",      preset.Arpeggiator.bpm);
        printf("Filter Mode:    %04d\n",      preset.Arpeggiator.fMode);
        printf("Octave Mode:    %04d\n\n",    preset.Arpeggiator.octMode);

        printf("----------------------------------------------------\n");
        printf("                      FX\n");
        printf("----------------------------------------------------\n\n");

        printf("Gain:           %04d\n\n",      preset.Effects.gain);
    }

    void savePreset(uint8_t slot, PRESET &preset) {
        if (slot >= MAX_PRESETS) {
            printf("ERROR! Outside of Preset storage range!\n");
            return;
        }

        uint16_t address = slot * PRESET_SIZE;
        uint8_t buffer[PAGE_SIZE] = {0};

        printf("Saving Preset %d ", slot);

        // the manual seperation of data is to allow a more effcient storage... will be changed when the preset organisation is improved.

        buffer[0] = (preset.Wave.shape >> 8) & 0xFF;
        buffer[1] = preset.Wave.shape & 0xFF;
        buffer[2] = (preset.Wave.vector >> 8) & 0xFF;
        buffer[3] = preset.Wave.vector & 0xFF;
        buffer[4] = (preset.Wave.octave >> 8) & 0xFF;
        buffer[5] = preset.Wave.octave & 0xFF;
        buffer[6] = (preset.Wave.pitch >> 8) & 0xFF;
        buffer[7] = preset.Wave.pitch & 0xFF;

        buffer[8] = (preset.Envelope.attack >> 8) & 0xFF;
        buffer[9] = preset.Envelope.attack & 0xFF;
        buffer[10] = (preset.Envelope.decay >> 8) & 0xFF;
        buffer[11] = preset.Envelope.decay & 0xFF;
        buffer[12] = (preset.Envelope.sustain >> 8) & 0xFF;
        buffer[13] = preset.Envelope.sustain & 0xFF;
        buffer[14] = (preset.Envelope.release >> 8) & 0xFF;
        buffer[15] = preset.Envelope.release & 0xFF;

        buffer[16] = preset.Modulation.state;
        buffer[17] = (preset.Modulation.matrix >> 8) & 0xFF;
        buffer[18] = preset.Modulation.matrix & 0xFF;
        buffer[19] = (preset.Modulation.rate >> 8) & 0xFF;
        buffer[20] = preset.Modulation.rate & 0xFF;
        buffer[21] = (preset.Modulation.depth >> 8) & 0xFF;
        buffer[22] = preset.Modulation.depth & 0xFF;
        buffer[23] = (preset.Modulation.wave >> 8) & 0xFF;
        buffer[24] = preset.Modulation.wave & 0xFF;

        buffer[25] = preset.Arpeggiator.state;
        buffer[26] = (preset.Arpeggiator.gate >> 8) & 0xFF;
        buffer[27] = preset.Arpeggiator.gate & 0xFF;
        buffer[28] = (preset.Arpeggiator.divisions >> 8) & 0xFF;
        buffer[29] = preset.Arpeggiator.divisions & 0xFF;
        buffer[30] = (preset.Arpeggiator.range >> 8) & 0xFF;
        buffer[31] = preset.Arpeggiator.range & 0xFF;
        buffer[32] = (preset.Arpeggiator.direction >> 8) & 0xFF;
        buffer[33] = preset.Arpeggiator.direction & 0xFF;

        buffer[34] = preset.Filter.state;
        buffer[35] = (preset.Filter.cutoff >> 8) & 0xFF;
        buffer[36] = preset.Filter.cutoff & 0xFF;
        buffer[37] = (preset.Filter.resonance >> 8) & 0xFF;
        buffer[38] = preset.Filter.resonance & 0xFF;
        buffer[39] = (preset.Filter.punch >> 8) & 0xFF;
        buffer[40] = preset.Filter.punch & 0xFF;
        buffer[41] = (preset.Filter.type >> 8) & 0xFF;
        buffer[42] = preset.Filter.type & 0xFF;

        buffer[43] = (preset.Filter.attack >> 8) & 0xFF;
        buffer[44] = preset.Filter.attack & 0xFF;
        buffer[45] = (preset.Filter.decay >> 8) & 0xFF;
        buffer[46] = preset.Filter.decay & 0xFF;
        buffer[47] = (preset.Filter.sustain >> 8) & 0xFF;
        buffer[48] = preset.Filter.sustain & 0xFF;
        buffer[49] = (preset.Filter.release >> 8) & 0xFF;
        buffer[50] = preset.Filter.release & 0xFF;

        buffer[51] = (preset.Effects.gain >> 8) & 0xFF;
        buffer[52] = preset.Effects.gain & 0xFF;
        buffer[53] = (preset.Arpeggiator.bpm >> 8) & 0xFF;
        buffer[54] = preset.Arpeggiator.bpm & 0xFF;
        buffer[55] = 0;
        buffer[56] = 0;
        buffer[57] = 0;
        buffer[58] = 0;
        buffer[59] = 0;
        buffer[60] = 0;

        buffer[61] = 0;
        buffer[62] = 0;
        buffer[63] = 0xAF;

        // maybe add some sort of padding here? 
        // 0xAF = After
        // 0xBE = Before

        write(address, buffer, PAGE_SIZE);

        

        printPresetData(preset);
    }

    void loadPreset (uint8_t slot, PRESET &preset) {
        // uint8_t setPreset, setBank;
        // setPreset = slot & 0xf;
        // setBank = (slot >> 4);

        if (slot >= MAX_PRESETS) {
            printf("ERROR! Outside of Preset storage range!\n");
            return;
        }
        
        uint16_t address = slot * PRESET_SIZE;
        uint8_t preset_buffer[PAGE_SIZE] = {0};

        printf("Loading Preset %d ", slot);

        read(address, preset_buffer, PAGE_SIZE);

        preset.Wave.shape               = (preset_buffer[0] << 8) | preset_buffer[1];
        preset.Wave.vector              = (preset_buffer[2] << 8) | preset_buffer[3];
        preset.Wave.octave              = (preset_buffer[4] << 8) | preset_buffer[5];
        preset.Wave.pitch               = (preset_buffer[6] << 8) | preset_buffer[7];

        preset.Envelope.attack          = (preset_buffer[8] << 8) | preset_buffer[9];
        preset.Envelope.decay           = (preset_buffer[10] << 8) | preset_buffer[11];
        preset.Envelope.sustain         = (preset_buffer[12] << 8) | preset_buffer[13];
        preset.Envelope.release         = (preset_buffer[14] << 8) | preset_buffer[15];

        preset.Modulation.state         = preset_buffer[16];
        preset.Modulation.matrix        = (preset_buffer[17] << 8) | preset_buffer[18];
        preset.Modulation.rate          = (preset_buffer[19] << 8) | preset_buffer[20];
        preset.Modulation.depth         = (preset_buffer[21] << 8) | preset_buffer[22];
        preset.Modulation.wave          = (preset_buffer[23] << 8) | preset_buffer[24];

        preset.Arpeggiator.state        = preset_buffer[25];
        preset.Arpeggiator.gate         = (preset_buffer[26] << 8) | preset_buffer[27];
        preset.Arpeggiator.divisions    = (preset_buffer[28] << 8) | preset_buffer[29];
        preset.Arpeggiator.range        = (preset_buffer[30] << 8) | preset_buffer[31];
        preset.Arpeggiator.direction    = (preset_buffer[32] << 8) | preset_buffer[33];

        preset.Arpeggiator.rest         = (preset_buffer[53] << 8) | preset_buffer[54];
        preset.Arpeggiator.bpm          = (preset_buffer[55] << 8) | preset_buffer[56];
        preset.Arpeggiator.fMode        = (preset_buffer[57] << 8) | preset_buffer[58];
        preset.Arpeggiator.octMode      = (preset_buffer[59] << 8) | preset_buffer[60];

        preset.Filter.state             = preset_buffer[34];
        preset.Filter.cutoff            = (preset_buffer[35] << 8) | preset_buffer[36];
        preset.Filter.resonance         = (preset_buffer[37] << 8) | preset_buffer[38];
        preset.Filter.punch             = (preset_buffer[39] << 8) | preset_buffer[40];
        preset.Filter.type              = (preset_buffer[41] << 8) | preset_buffer[42];

        preset.Filter.attack            = (preset_buffer[43] << 8) | preset_buffer[44];
        preset.Filter.decay             = (preset_buffer[45] << 8) | preset_buffer[46];
        preset.Filter.sustain           = (preset_buffer[47] << 8) | preset_buffer[48];
        preset.Filter.release           = (preset_buffer[49] << 8) | preset_buffer[50];

        preset.Effects.gain             = (preset_buffer[51] << 8) | preset_buffer[52];


        printPresetData(preset);
    }
    
    void test_save (PRESET &preset) {
        // Set up a Union for data splitting...
        union MyUnion {
            PRESET p;
            uint8_t array[PAGE_SIZE];

            MyUnion(PRESET input) : p(input) {}
            MyUnion(const uint8_t* inputArray) {
                // Use a pointer to directly access and copy data from the input array
                memcpy(&p, inputArray, sizeof(PRESET));
            }
        };
        // Copy the data into it
        MyUnion temp(preset);

        
        // set the test address...
        uint16_t test_address = FREE_ADDRESS + 128;

        printf("Attempting to write test data...\n");

        // write it to the eeprom
        EEPROM::write(test_address, temp.array, PAGE_SIZE);

        uint8_t temp_buffer[PAGE_SIZE];
        PRESET read;

        printf("Attempting to read test data...\n");
        // read it from the eeprom...
        EEPROM::read(test_address, temp_buffer, PAGE_SIZE);

        MyUnion read_data(temp_buffer);

        printPresetData(read_data.p);

        // print it out to check it...

    }
    void clearPreset (uint8_t slot) {
        uint16_t address = slot * PAGE_SIZE;
        uint8_t buffer[PAGE_SIZE] = {0};

        printf("Clearing Preset %d in EEPROM address: %d...     ", slot, address);

        write(address, buffer, PAGE_SIZE);

        printf("Cleared!\n", slot);
    }
    
    void transferPreset (uint16_t fromAddress, uint16_t toAddress) {

        printf("Starting Preset transfer...\n");

        // Create an array to use as a buffer
        uint8_t transfer_buffer[PAGE_SIZE];
        
        // Read the data from the EEPROM
        read(fromAddress, transfer_buffer, PAGE_SIZE);
        
        printf("Preset loaded into buffer!\n");

        write(toAddress, transfer_buffer, PAGE_SIZE);

        printf("Preset transfer complete!\n");
        
    }

    void restoreFactoryPreset (uint8_t slot) {
        if (slot >= MAX_PRESETS) {
            printf("ERROR! Outside of Factory Preset storage range!\n");
            return;
        }

        printf("\nCopying Factory Preset %d to Preset Location...   ", slot);
        // load Factory Preset from EEPROM into buffer
        uint16_t factory_preset_address = (slot * PRESET_SIZE) | (FACTORY_PRESET_ADDRESS);
        uint16_t output_address = slot * PRESET_SIZE;
        
        transferPreset(factory_preset_address, output_address);
    }
    
    void writeFactoryPreset (uint8_t slot) {
        if (slot >= MAX_PRESETS) {
            printf("ERROR! Outside of Factory Preset storage range!\n");
            return;
        }
        // store Preset to Factory Preset area in EEPROM
        printf("\nCopying Preset %d to Factory Preset Location...\n", slot);
        // take the current preset with standard page size (64 byte)
        uint16_t preset_address = slot * PRESET_SIZE;

        // send it to the new Factory preset space, with new page size (128 bytes)
        uint16_t factory_preset_address = (slot * PRESET_SIZE) | (FACTORY_PRESET_ADDRESS);

        transferPreset(preset_address, factory_preset_address);

        printf("Factory Preset %d saved.\n\n", slot);

    }
}