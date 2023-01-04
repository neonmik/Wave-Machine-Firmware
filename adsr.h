#include "log.h"   // log/anti-log table lookup

#define STOPPED 0
#define ATTACK 1
#define DECAY 2
#define SUSTAIN 3
#define RELEASE 4
#define FINISHED 5


#define VOICES 0
#define FILTER 1


// these dont work right yet... if i put them up (even to 5) it freaks and does the same scale but over a shorter period on th pots
#define max_attack_time 2  //  max Attack setting in seconds (ish).
#define max_decay_time 2   //  max Decay setting in seconds (ish).
#define max_release_time 2 //  max Release setting in seconds (ish).


#define minimum_scale_value 2

struct adsr_struct {
  unsigned long envelope_index;
  volatile uint32_t stage;
  volatile uint16_t release_note;
  volatile uint16_t release_gain; // the last value, when note off came
  volatile bool gate = 0;
  volatile uint16_t output;
};

struct adsr_values_struct {
  volatile uint16_t attack_step;      // attack step 
  volatile uint16_t decay_step;      // decay step
  volatile uint16_t sustain_value;  // sustain value
  volatile uint16_t release_step;      // release step
};

adsr_struct oscillator_adsr[MAX_VOICES+1]; // setting up ADSR for oscillators (and Filter)
adsr_values_struct adsr_values[2]; // setting up global envelope lengths (will be used for Envelope and Filter stages later)

uint32_t voice_output[MAX_VOICES];
uint32_t envelope_output[MAX_VOICES];
uint32_t active_voices[MAX_VOICES];
unsigned long virtual_index_max;

void set_adsr_stage (int stage, int voice);
void set_adsr_values();
void set_gate_on (int voice);
void set_gate_off (int voice);
uint16_t get_attack_value(int16_t voice);
uint16_t get_decay_value(int16_t voice);
void get_sustain_value(uint16_t f, int16_t voice);
uint16_t get_table_index(int16_t voice);
void add_envelope_step (int voice, int mode);
void set_attack_scale(uint16_t scale_value, int16_t voice);
void set_decay_scale(uint16_t scale_value, int16_t voice);
void set_release_scale(uint16_t scale_value, int16_t voice);
uint16_t calcuate_scale(uint16_t scale_value, uint16_t scale_time);

void set_adsr_values() {
    set_attack_scale(0, VOICES);
    set_decay_scale(0, VOICES);
    get_sustain_value(127, VOICES);
    set_release_scale(0, VOICES);

    // ADSR vector synthesis:
    // set_attack_scale(511, FILTER);
    // set_decay_scale(511, FILTER);
    // get_sustain_value(196, FILTER);
    // set_release_scale(511, FILTER);
}

void init_adsr() {
  for (int voice = 0; voice < MAX_VOICES; voice++) {
    set_adsr_stage(STOPPED, voice);
    oscillator_adsr[voice].output = 0;
  }
  // Init ADSR Value sets
  for (int type = 0; type < 1; type++) {
    adsr_values[type].attack_step = 0;
    adsr_values[type].decay_step = 0;
    adsr_values[type].sustain_value = 0;
    adsr_values[type].release_step = 0;
  }
  virtual_index_max = (log_length - 1); 

  set_adsr_values();
}

void set_adsr_stage (int stage, int voice) {
  oscillator_adsr[voice].stage = stage;
}

void set_gate_on (int voice) {
  // marks the oscillator as active
  oscillator_adsr[voice].gate = 1;
  oscillator_adsr[voice].envelope_index = 0;
  oscillators[voice].is_active = 1;
  if (oscillator_adsr[voice].stage != STOPPED) {

  }
  set_adsr_stage (ATTACK, voice);

  // ADSR Vector Synthesis
  // set_adsr_stage (ATTACK, MAX_VOICES);
}

void set_gate_off (int voice) {
  // set the starting gain level for the release stage from the current voice 
  // reset Envelope index
  oscillator_adsr[voice].gate = 0;
  oscillator_adsr[voice].release_gain = oscillator_adsr[voice].output; // the initial amplitude value for the release mode
  oscillator_adsr[voice].envelope_index = 0;
  set_adsr_stage(RELEASE, voice);
}

uint16_t get_attack_value(int16_t voice) {
  return get_attack(get_table_index(voice));
}
 
uint16_t get_decay_value(int16_t voice) {
  return get_decay(get_table_index(voice));
}



void get_sustain_value(uint16_t vol, int16_t voice) {
  adsr_values[voice].sustain_value = vol;
}

uint16_t get_table_index(int16_t voice) {
  return uint16_t (oscillator_adsr[voice].envelope_index); // remember: the tableIndex was multiplied by 1024
}


void add_envelope_step (int voice, int mode) { // switch function to set switching between stages... mainly useful for Decay/Sustain stages
  switch (oscillator_adsr[voice].stage) {
    case ATTACK:
      oscillator_adsr[voice].envelope_index += adsr_values[mode].attack_step;
      if (oscillator_adsr[voice].envelope_index >= virtual_index_max) {
        set_adsr_stage(DECAY, voice);
        oscillator_adsr[voice].envelope_index = 0;
      }
      
      else {
        if (oscillator_adsr[voice].output >= 255) { // Check first for efficiancy
          set_adsr_stage(DECAY, voice); // switch to Sustain mode
          oscillator_adsr[voice].envelope_index = 0;
        }
        oscillator_adsr[voice].output = get_attack_value(voice);
      }
    break;
    
    case DECAY:
      oscillator_adsr[voice].envelope_index += adsr_values[mode].decay_step; 
      if (oscillator_adsr[voice].envelope_index >= virtual_index_max) {
        set_adsr_stage(SUSTAIN, voice);
        oscillator_adsr[voice].envelope_index = 0;
      }
      else {
        if (oscillator_adsr[voice].output <= adsr_values[mode].sustain_value) { // Check first for efficiancy
          set_adsr_stage(SUSTAIN, voice); // switch to Sustain mode
          oscillator_adsr[voice].envelope_index = 0;
        }
        else oscillator_adsr[voice].output = get_decay_value(voice); 
      }
    break;
    
    case SUSTAIN:
      oscillator_adsr[voice].output = adsr_values[mode].sustain_value;
      
    break;
    
    case RELEASE:
      oscillator_adsr[voice].envelope_index += adsr_values[mode].release_step; 
      if (oscillator_adsr[voice].envelope_index >= virtual_index_max) {
          oscillator_adsr[voice].stage = FINISHED; // switch off
      }
      else {
        uint16_t releaseDecay = get_decay_value(voice);   // table value (4096-0)
        releaseDecay = (releaseDecay * oscillator_adsr[voice].release_gain) / log_length;    // weighted with the sustain value
        if ( releaseDecay <= 1){
          releaseDecay = 0;
          oscillator_adsr[voice].stage = FINISHED; // switch off
        }
        oscillator_adsr[voice].output = releaseDecay; 
      }
    break;
    case FINISHED:
      oscillator_adsr[voice].envelope_index = 0;

      oscillators[voice].note = 0;

      oscillators[voice].is_active = 0;

      oscillator_adsr[voice].output = 0;
      oscillator_adsr[voice].release_gain = 0;
      oscillator_adsr[voice].stage = STOPPED;
    break;
  }  
  

}


// --------------------------------------------------------------------------- //
// ------------------------- Setting Up ADSR lengths ------------------------- //
// --------------------------------------------------------------------------- //

void set_attack_scale(uint16_t scale_value, int16_t voice) {
  if (scale_value <= minimum_scale_value) scale_value = minimum_scale_value;
  adsr_values[voice].attack_step = calcuate_scale(scale_value, max_attack_time);
}

void set_decay_scale(uint16_t scale_value, int16_t voice) {
  if (scale_value <= minimum_scale_value) scale_value = minimum_scale_value;
  adsr_values[voice].decay_step = calcuate_scale(scale_value, max_decay_time);
}

void set_release_scale(uint16_t scale_value, int16_t voice) {
  if (scale_value <= minimum_scale_value) scale_value = minimum_scale_value;

  adsr_values[voice].release_step = calcuate_scale(scale_value, max_release_time);
}


// ***************************************************************************
// * The table goes from 0 to 4096, attack value determines                  *
// * how fast we travel trough the table.                                    *
// * In the end we always should reach the table-end (4096)                  *
// * The Formula for Increments/ISRTick is:                                  *
// * Increment/Tick  z = Tablelength^2 / (scale_value * TableTime * IRSRate)  *
// * A scale of 0 = instant attack, 4096 = longest Attack Time               *
// * @param scale_value the new attack value                                  *
// * @param channel voices or filter setting                                 *
// ***************************************************************************

// *******************************************************************
// * Calculate the step with which we travel in the adsr tables      *
// * Analog value goes from 0-4095                                   *
// * The log-table goes from 0-4095, the values in there from 0-255  *
// ******************************************************************* 


uint16_t calcuate_scale(uint16_t scale_value, uint16_t scale_max) {
  unsigned long z = scale_length / scale_value; // envelope tabele length
  unsigned long n = scale_max * ENV_RATE; // scale_max * ENV_RATE (or... Total Seconds * Envelope Rate)
  z /= n;

  return (uint16_t) z+1; // adding 1 cause if it the pot's too high (which happens if the knobs past 12) the release hangs and the attack doesnt atack... as the result of the equation is 0 (basically the result is a float?).
}

void update_adsr () {
  for (int i = 0; i < MAX_VOICES; i++) {
    if (oscillator_adsr[i].stage != STOPPED) add_envelope_step(i, VOICES);     // 0-4096  all adsr ???
  }
  // ADSR Vector Synthesis
  // if (oscillator_adsr[MAX_VOICES].stage != STOPPED) add_envelope_step(MAX_VOICES, FILTER);
}