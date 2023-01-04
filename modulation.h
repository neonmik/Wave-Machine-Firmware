// This wants to be a LFO for modulating different sources...
// I think it should be between 1Hz-20Hz (Or ???100Hz???)
// Could be used for:
//                      - Vibrato (Note frequency modulations)
//                      - Tremelo (Note Volume Modulations)
//                      - Wave Vector Modulations
//                      - A Filter?

uint16_t mod_index = 0;
uint32_t mod_increment = 0;
uint16_t mod_acc = 0;
uint16_t mod_output = 0;

uint16_t mod_wave = 0;
uint16_t mod_depth = 0;
uint16_t mod_rate = 0;

uint8_t env_rate = 256; //figure this out

void set_mod_depth(int depth) {
    mod_depth = depth;
}

void set_mod_rate(int rate) {
    mod_rate = ((65536 * (rate)) / env_rate)>>6;
}

void set_mod_wave(int waveform) {
    mod_wave = waveform*256;
}

void init_mod (void) {
    set_mod_depth(0);
    set_mod_rate(0);
    set_mod_wave(0);
}

void update_mod () {
    mod_acc += mod_rate;
    mod_index = mod_acc >> 8;
    mod_output = ((get_modulation(mod_index)*mod_depth)>>8);
    // wave_vector += mod_output;
    // Serial.print("mod_rate: ");
    // Serial.println(mod_rate);
    // Serial.print("mod_acc: ");
    // Serial.println(mod_acc);
    // Serial.print("mod_index: ");
    // Serial.println(mod_index);
    // Serial.print("mod_output: ");
    // Serial.println(mod_output);
}

