#pragma once

#include <cstdint>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

#include "../debug.h"
#include "../config.h"               


#include "adsr.h"

#include "wavetable.h"
#include "log_table.h"

#include "../mailbox.h"


namespace SYNTH {

  namespace {
    const uint32_t note2freq[128] = {
        535809,	    567670,	    601425,	    637188,	    675077,	    715219,	    757748,	    802806,
        850544,	    901120,	    954703,	    1011473,	1071618,	1135340,	1202850,	1274376,
        1350154,	1430438,	1515497,	1605613,	1701088,	1802240,	1909406,	2022946,
        2143236,	2270680,	2405701,	2548752,	2700308,	2860877,	3030994,	3211226,
        3402176,	3604480,	3818813,	4045892,	4286473,	4541360,	4811403,	5097504,
        5400617,	5721755,	6061988,	6422453,	6804352,	7208960,	7637627,	8091784,
        8572946,	9082720,	9622807,	10195009,	10801235,	11443510,	12123977,	12844906,
        13608704,	14417920,	15275254,	16183568,	17145893,	18165440,	19245614,	20390018,
        21602471,	22887021,	24247954,	25689812,	27217408,	28835840,	30550508,	32367136,
        34291786,	36330881,	38491228,	40780036,	43204943,	45774042,	48495908,	51379625,
        54434817,	57671680,	61101016,	64734272,	68583572,	72661763,	76982456,	81560072,
        86409886,	91548085,	96991817,	102759251,	108869634,	115343360,	122202033,	129468544,
        137167144,	145323527,	153964913,	163120144,	172819772,	183096171,	193983635,	205518503,
        217739269,	230686720,	244404066,	258937088,	274334288,	290647054,	307929827,	326240288,
        345639545,	366192342,	387967271,	411037006,	435478538,	461373440,	488808132,	517874176,
        548668577,	581294108,	615859655,	652480576,	691279090,	732384684,	775934543,	822074012,
    };
    inline uint32_t get_freq(uint8_t note) {
        return note2freq[note];
    }

  }

  extern uint16_t volume;

  enum Oscillator {
    WAVETABLE = 256,
    NOISE     = 128,
    SQUARE    = 64,
    SAW       = 32,
    TRIANGLE  = 16,
    SINE      = 8,
    WAVE      = 1
  };
  
  // used oscillator types, this can use multiple oscillators, although can't be currently adjusted by the hardware
  extern uint16_t   _oscillator;      // bitmask for enabled oscillator types (see Oscillator enum for values)

  // variables for the wavetable oscillator
  extern uint16_t   _wave_shape;
  extern uint16_t   _wave_vector;
  extern uint16_t   _vector_mod;

  extern uint32_t   _attack;      // attack period - moved to global as it's not needed per voice for this implementation.
  extern uint32_t   _decay;      // decay period
  extern uint32_t   _sustain;   // sustain volume
  extern uint32_t   _release;      // release period
  extern uint16_t   _last_attack;      // attack period - moved to global as it's not needed per voice for this implementation.
  extern uint16_t   _last_decay;      // decay period
  extern uint16_t   _last_sustain;   // sustain volume
  extern uint16_t   _last_release;      // release period

  extern int16_t    _vibrato;
  extern uint16_t   _tremelo;

  extern uint16_t   _pitch_scale;

  extern uint8_t _octave;

  static bool _soft_start = true;
  static uint8_t  _soft_start_index = 0;
  static int16_t _soft_start_sample = -32768;

  extern bool      filter_enable;
  extern uint16_t  filter_cutoff_frequency;

  struct Oscillators {

    uint16_t  volume        = 0x7fff;    // channel volume (default 50%) - also could be called velocity


    bool      _gate         = false;  // used for tracking a note that's released, but not finished.
    bool      _active       = false;  // used for whole duration of note, from the very start of attack right up until the voise is finished
    uint32_t  activation_time  = 0;

    uint8_t   _note         = 0;
    uint32_t  _frequency    = 0;    // frequency of the voice (Hz)

    uint16_t  pulse_width   = 0x7fff; // duty cycle of square wave (default 50%)
    int16_t   noise         = 0;      // current noise value

    uint32_t  waveform_offset  = 0;   // voice offset (Q8)

    uint8_t   wave_buf_pos  = 0;      //
    int16_t   wave_buffer[64];        // buffer for arbitrary waveforms. small as it's filled by user callback

    void *user_data = nullptr;
    void (*wave_buffer_callback)(Oscillator &channel);

    void note_on (uint8_t note) {
      _gate = true; // wouldn't be needed if core moved
      _active = true;

      _note = note;
      _frequency = get_freq(note);
    

      ADSR.trigger_attack();
    }
    void note_off (void) {
      _gate = false; // wouldn't be needed if core moved
      ADSR.trigger_release();
    }
    void note_stopped (void) {
      _active = false;
      _note = 0;
      _frequency = 0;
    }
    
    ADSREnvelope ADSR{_attack, _decay, _sustain, _release};
  };

  extern Oscillators channels[MAX_VOICES];

  void voice_on (uint8_t voice, uint8_t note);
  void voice_off (uint8_t voice);
  
  uint16_t get_audio_frame();
  bool is_audio_playing();
  void init ();

  void set_waveshape (uint16_t shape);
  void set_wavevector (uint16_t vector);
  void set_octave (uint16_t octave);
  void set_pitch_scale (uint16_t scale);
  uint16_t get_pitch_log (uint16_t index);

  void set_attack (uint16_t attack);
  void set_decay (uint16_t decay);
  void set_sustain (uint16_t sustain);
  void set_release (uint16_t release);
  
  uint32_t calc_end_frame (uint32_t milliseconds);

  void modulate_vibrato (uint16_t vibrato);
  void modulate_tremelo (uint16_t tremelo);
  void modulate_vector (uint16_t vector_mod);

}
