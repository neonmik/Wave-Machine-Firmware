struct oscillator  {
  volatile uint32_t frequency;        // used to work out frequency from note number
  volatile int note;                  // used for MIDI note

  volatile uint32_t increment;        // increment  = (FrequencyInHertz * 65536) / SampleRate
                                      // not sure if this equation is correct still, but basically used to tell the oscillator how far to push the phase Index through the table for each pitch

  volatile uint16_t phase_acc;         // keeps track of the phase

  volatile int waveform;              // waveform of the oscillator (helps with the pointer for the waveform table)
  volatile int wave_index = 0;         // Index for wave lookup (the upper 8 bits of the accumulator)
  volatile int wave_vector = 0;        // VERY COOL used to modulate the waveforms (via Vector synthesis - used in the Sequential Circuits Prophet-VS and Korg Wavestation)

  volatile uint8_t gain = 0xff;       // 256 volume levels (for MIDI velocity when I implement it)

  volatile uint16_t output;           // final oscillator output

  volatile bool is_active = 0;         // used to check if the oscillator is assigned
  volatile unsigned long activation_time = 0; // used to check the oldest note
};
oscillator oscillators[voices];

bool mix_flag = 0;
uin8_t max_voices = 0;

uint8_t octave = 0;
// final sample that goes to the DAC
uint32_t sample = 0;

//in the highest octave, so divide by 2^x to shift octaves.
//increment = (65536 * frequency) / sampleRate;
//http://tonalsoft.com/pub/news/pitch-bend.aspx

const int note_increments[12] = { // 44100hz
 //increment     note        Hz           midi note
 6221,        // C8       4186.009      108
 6591,        // C#8      4434.922      109
 6983,        // D8       4698.636      110
 7398,        // D#8      4978.032      111
 7838,        // E8       5274.041      112
 8304,        // F8       5587.652      113
 8797,        // F#8      5919.911      114
 9321,        // G8       6271.926      115
 9875,        // G#8      6644.875      116
 10462,        // A8       7040.000      117
 11084,        // A#8      7458.620      118
 11743         // B8       7902.133      119
};


const int note_frequency[12] = {    //C7 - B7
  2093, 2217, 2349,    // other octaves by dividing by power of 2
  2489, 2637, 2794,
  2960, 3136, 3322,
  3520, 3729, 3951
};

const uint16_t midi_frequency[120] = {
//C,    C',   D,    D',   E,     F,     F',    G,     G',    A,     A',    B  
  8,    9,    9,    10,   10,    11,    12,    12,    13,    14,    15,    16, 
  17,   18,   19,   21,   22,    23,    24,    25,    26,    27,    29,    31,
  33,   35,   37,   39,   41,    44,    46,    49,    52,    55,    58,    62,
  65,   69,   73,   78,   82,    87,    92,    98,    104,   110,   117,   123,
  131,  139,  147,  156,  165,   175,   185,   196,   208,   220,   233,   247,
  262,  277,  294,  311,  330,   349,   370,   392,   415,   440,   466,   494,
  523,  554,  587,  622,  659,   698,   740,   784,   831,   880,   932,   988,
  1047, 1109, 1175, 1245, 1319,  1397,  1480,  1568,  1661,  1760,  1865,  1976,
  2093, 2217, 2349, 2489, 2637,  2794,  2960,  3136,  3322,  3520,  3729,  3951,
  4186, 4435, 4699, 4978, 5274,  5588,  5920,  6272,  6645,  7040,  7459,  7902
};

int wave_knob;

uint8_t current_notes[max_voices];
uint8_t last_written = 0;  //the last changed MIDI note slot out of the 8
int voice_counter = 0;

#include "adsr.h"
#include "waveforms.h"
#include "modulation.h"

// ----------------------------------------------------------------------------------- //
// ----------------------------- Oscillator Calculations ----------------------------- //
// ----------------------------------------------------------------------------------- //

void init_oscillators (uint16_t sample_rate, uint8_t voices) {
  set_all_waveform(0);
  max_voices = voices;
  oscillator oscillators[voices];  // setting up oscillators[]
}

uint16_t midi_note_to_freq(uint8_t note) {
  uint16_t Hz = (midi_frequency[note]);
  return Hz;
}

void set_waveform(uint8_t oscillator, uint8_t waveform)  {
  oscillators[oscillator].waveform = (waveform*256);
}

void set_all_waveform(uint8_t waveform)  {
  for(uint8_t i = 0; i < max_voices; i++)  {
    set_waveform(i, waveform);
  }
}

void set_wavevector(uint8_t oscillator, int wavevector)  {
  oscillators[oscillator].wave_vector = wavevector;
}

void set_all_wavevector(int wavevector)  {
  for(uint8_t i = 0; i < max_voices; i++)  {
    set_waveform(i, wavevector);
  }
}

void set_oscillators (int voice, int note, int vel) {
  oscillators[voice].note = note;
  oscillators[voice].frequency = midi_note_to_freq(note);
  oscillators[voice].gain = vel;
}

void clear_oscillators (int voice) {
  oscillators[voice].note = 0;
  oscillators[voice].frequency = 0;
  oscillators[voice].increment = 0;
  oscillators[voice].output = 0;
  oscillators[voice].gain = 0;
}

void stop_oscillators (int voice) {
  oscillator_adsr[voice].stage = FINISHED;
}

void clear_all_oscillators () {
  for (int i = 0; i < max_voices; i++) {
    clear_oscillators(i);
  }
}



void calc_oscillator (int voice) {
  if (wave_vector) oscillators[voice].wave_vector = wave_vector + mod_output; // For Modulated Vector Synthesis: + mod_output; 
                                                        // For ADSR Vector Synthesis:  + oscillator_adsr[max_voices].output);
  else oscillators[voice].wave_vector = 0;
  oscillators[voice].increment = (((oscillators[voice].frequency * pitchScale) >> 9)>> octave); // pitchScale works for 1 octave +/-, but is the wrong note... and slightly out ouf tune.
  oscillators[voice].increment = (65536 * oscillators[voice].increment) / sampleRate; // to convert the frequency to increment: increment = (65536 * frequency) / sampleRate;
  oscillators[voice].phase_acc += oscillators[voice].increment;  // add in pith, the higher the number, the faster it rolls over, the more cycles per second
  oscillators[voice].wave_index = oscillators[voice].phase_acc >> 8;   // use top 8 bits as wavetable wave_index
  oscillators[voice].output = getWavetable(voice);    // get sample from wave table 
                                                      // For Modulated Tremelo: *(mod_output>>4)
  activeVoice[voice] = (oscillator_adsr[voice].output * oscillators[voice].output); // Used to have /log_length on the end...  Not sure why, but we dont have to do this anymore...
                                                                                             // For softer ModTrem: ((oscillator_adsr[voice].output-(mod_output>>4)) * oscillators[voice].output)
  sample += activeVoice[voice];  // the max this can be is a 16 bit number
}





uint32_t get_audio_frame (void) {
  sample = 0x7FF;       // Sample to pass out to DAC
  for (int i = 0; i < max_voices; i++) { // Voice amount for loop
    if (oscillator_adsr[i].stage != STOPPED) { //If the ADSR for the voice is not Stopped or Stopping
      calc_oscillator(i);                 // Audio wave, each 
    }
  }
  return sample;
}
