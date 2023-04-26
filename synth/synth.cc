#include "synth.h"
#include <math.h>

#include "modulation.h"

namespace SYNTH {



  uint16_t oscillator = Oscillator::WAVETABLE; // | Oscillator::TRIANGLE;      // bitmask for enabled waveforms (see AudioWaveform enum for values)

  uint16_t _wave_shape;
  uint16_t _last_shape;

  uint16_t _wave_vector;
  uint16_t _vector_mod;


  uint32_t  _attack;
  uint32_t  _decay;
  uint32_t  _sustain;
  uint32_t  _release;
  uint16_t   _last_attack;
  uint16_t   _last_decay;
  uint16_t   _last_sustain;
  uint16_t   _last_release;

  int16_t   _vibrato;
  uint16_t  _tremelo;

  uint16_t _pitch_scale = 511;
  uint16_t _last_pitch;

  uint8_t _octave = 0;
  uint16_t  _last_octave;

  // bool      filter_enable = true;
  // uint16_t  filter_cutoff_frequency = 1000;
  // float filter_epow = 1 - expf(-(1.0f / 44100.0f) * 2.0f * pi * int32_t(filter_cutoff_frequency));

  uint16_t volume = 0x6fff;
  const int16_t sine_waveform[256] = {-32768,-32758,-32729,-32679,-32610,-32522,-32413,-32286,-32138,-31972,-31786,-31581,-31357,-31114,-30853,-30572,-30274,-29957,-29622,-29269,-28899,-28511,-28106,-27684,-27246,-26791,-26320,-25833,-25330,-24812,-24279,-23732,-23170,-22595,-22006,-21403,-20788,-20160,-19520,-18868,-18205,-17531,-16846,-16151,-15447,-14733,-14010,-13279,-12540,-11793,-11039,-10279,-9512,-8740,-7962,-7180,-6393,-5602,-4808,-4011,-3212,-2411,-1608,-804,0,804,1608,2411,3212,4011,4808,5602,6393,7180,7962,8740,9512,10279,11039,11793,12540,13279,14010,14733,15447,16151,16846,17531,18205,18868,19520,20160,20788,21403,22006,22595,23170,23732,24279,24812,25330,25833,26320,26791,27246,27684,28106,28511,28899,29269,29622,29957,30274,30572,30853,31114,31357,31581,31786,31972,32138,32286,32413,32522,32610,32679,32729,32758,32767,32758,32729,32679,32610,32522,32413,32286,32138,31972,31786,31581,31357,31114,30853,30572,30274,29957,29622,29269,28899,28511,28106,27684,27246,26791,26320,25833,25330,24812,24279,23732,23170,22595,22006,21403,20788,20160,19520,18868,18205,17531,16846,16151,15447,14733,14010,13279,12540,11793,11039,10279,9512,8740,7962,7180,6393,5602,4808,4011,3212,2411,1608,804,0,-804,-1608,-2411,-3212,-4011,-4808,-5602,-6393,-7180,-7962,-8740,-9512,-10279,-11039,-11793,-12540,-13279,-14010,-14733,-15447,-16151,-16846,-17531,-18205,-18868,-19520,-20160,-20788,-21403,-22006,-22595,-23170,-23732,-24279,-24812,-25330,-25833,-26320,-26791,-27246,-27684,-28106,-28511,-28899,-29269,-29622,-29957,-30274,-30572,-30853,-31114,-31357,-31581,-31786,-31972,-32138,-32286,-32413,-32522,-32610,-32679,-32729,-32758};

  Voices channels[MAX_VOICES];

  void voice_on (uint8_t voice, uint8_t note, uint16_t frequency) {
    channels[voice].note_on(note, frequency);
  }
  void voice_off (uint8_t voice) {
    channels[voice].note_off();
  }

  // uint32_t prng_xorshift_state = 0x32B71700;

  // uint32_t prng_xorshift_next() {
  //   uint32_t x = prng_xorshift_state;
  //   x ^= x << 13;
  //   x ^= x >> 17;
  //   x ^= x << 5;
  //   prng_xorshift_state = x;
  //   return x;
  // }

  // int32_t prng_normal() {
  //   // rough approximation of a normal distribution
  //   uint32_t r0 = prng_xorshift_next();
  //   uint32_t r1 = prng_xorshift_next();
  //   uint32_t n = ((r0 & 0xffff) + (r1 & 0xffff) + (r0 >> 16) + (r1 >> 16)) / 2;
  //   return n - 0xffff;
  // }

  uint32_t _sample_rate;
  
  void init (uint32_t sample_rate) {
    _sample_rate = sample_rate;
  }

  
  bool is_audio_playing() {
    if(volume == 0) {
      return false;
    }

    bool any_channel_playing = false;
    for(int c = 0; c < MAX_VOICES; c++) {
      if(channels[c].volume > 0 && channels[c].ADSR.isStopped()) {
        any_channel_playing = true;
      }
    }

    return any_channel_playing;
  }

  uint16_t get_audio_frame() {
    
    if (_soft_start) {
      _soft_start_index++;
      if (_soft_start_index >= 2) {
        _soft_start_index = 0;
        _soft_start_sample += 1;
        if (_soft_start_sample >= 0) {
          _soft_start_sample = 0;
          _soft_start = false;
        }
      }
      return (_soft_start_sample+32767)>>4;
    } 

    else {
      int32_t sample = 0;  // used to combine channel output
      int16_t clipped_sample = 0;

      MOD::update();

      // implemented this here so that it's set for the whole sample run...
      uint16_t vector = (_wave_shape + (_wave_vector + _vector_mod));
      int32_t output_volume = (volume - _tremelo);

      // add mod in here? saves doing it 8 times in the voice (pretty sure it couldn't handle it) 
      // increment the LFO offset
      // if (MOD::_active) {
      //   _vector_mod += MOD::get_sample();
      
      
      for(int c = 0; c < MAX_VOICES; c++) {

        auto &channel = channels[c];

        
        // increment the waveform position counter. this provides an
        // Q16 fixed point value representing how far through
        // the current waveform we are
        channel.waveform_offset += (((((channel._frequency * _pitch_scale) >>9) << _octave) << 8) <<8) / _sample_rate; //try <<8 instead of *256... also might be easier to shift 16? check it

        //this is where vibrato is added... has to be here and not in the pitch scale as it would be lopsided due to logarithmic nature of freqencies.
        channel.waveform_offset += _vibrato;

        channel.ADSR.update();

        // if(channel.waveform_offset & 0x10000) {
        //   // if the waveform offset overflows then generate a new
        //   // random noise sample
        //   channel.noise = prng_normal();
        // }

        channel.waveform_offset &= 0xffff;

        // check if any waveforms are active for this channel
        if(channel._active) {
          uint8_t waveform_count = 0;
          int32_t channel_sample = 0;


          if (oscillator & Oscillator::NOISE) {
            channel_sample += channel.noise;
            waveform_count++;
          }

          if (oscillator & Oscillator::SAW) {
            channel_sample += (int32_t)channel.waveform_offset - 0x7fff;
            waveform_count++;
          }

          if (oscillator & Oscillator::TRIANGLE) {
            if (channel.waveform_offset < 0x7fff) { // initial quarter up slope
              channel_sample += int32_t(channel.waveform_offset * 2) - int32_t(0x7fff);
            }
            else { // final quarter up slope
              channel_sample += int32_t(0x7fff) - ((int32_t(channel.waveform_offset) - int32_t(0x7fff)) * 2);
            }
            waveform_count++;
          }

          if (oscillator & Oscillator::SQUARE) {
            channel_sample += (channel.waveform_offset < channel.pulse_width) ? 0x7fff : -0x7fff;
            waveform_count++;
          }
          
          if (oscillator & Oscillator::SINE) {
            // the sine_waveform sample contains 256 samples in
            // total so we'll just use the most significant bits
            // of the current waveform position to index into it
            channel_sample += sine_waveform[(channel.waveform_offset >> 8)];
            waveform_count++;
            
          }

          if (oscillator & Oscillator::WAVETABLE) {

            // the wavetable sample contains 256 samples in
            // total so we'll just use the most significant bits
            // of the current waveform position to index into it
            channel_sample += wavetable[(channel.waveform_offset >> 8) + vector];
            waveform_count++;

            // OLD Arbitary Waveform? Not sure how it was meant to work, but it didnt...
            // channel_sample += channel.wave_buffer[channel.wave_buf_pos];
            // if (++channel.wave_buf_pos == 64) {
            //   channel.wave_buf_pos = 0;
            //   if(channel.wave_buffer_callback)
            //       channel.wave_buffer_callback(channel);
            // }
            // waveform_count++;
          }

          // divide the sample by the amount of waveforms - good for multi oscillator voices
          channel_sample = channel_sample / waveform_count;
          
          // apply ADSR
          channel_sample = (int32_t(channel_sample) * int32_t((channel.ADSR.get_adsr()) >> 8)) >> 16;

          // apply channel volume
          channel_sample = (int32_t(channel_sample) * int32_t(channel.volume)) >> 16;

          // apply channel filter
          
          // if (filter_enable) {
          //   channel_sample += (channel_sample - channel.filter_last_sample) * filter_epow;
          // }

          // channel.filter_last_sample = channel_sample;

          sample += channel_sample;
        }
      }
      
      sample = (int32_t(sample) * output_volume) >> 16;

      //attempt at soft clipping - doesnt work
      // sample = ((sample + (sample>>1))-10) * sample - ((sample>>1)-10) * sample * sample * sample;

      // clip result to 16-bit
      sample = sample <= -0x8000 ? -0x8000 : (sample > 0x7fff ? 0x7fff : sample);
      // sample = lowPassFilter.process(sample);
      return (sample+32767)>>4;
    }

  }

  void set_waveshape (uint16_t shape) {
    if (shape == _last_shape) return;
    _last_shape = shape;
    _wave_shape = ((shape>>6)*256);
  }
  void set_wavevector (uint16_t vector) {
    _wave_vector = vector;
  }
  void set_octave (uint16_t octave) {
    if (octave == _last_octave) return;
    _octave = (octave>>8);
    _last_octave = octave;
  }
  void set_pitch_scale (uint16_t scale) {
    // if (scale == _last_pitch) return;
    _pitch_scale = get_pitch_log(scale);
    _last_pitch = scale;
  }
  uint16_t get_pitch_log (uint16_t index) {
      return log_table[index];
  }

  void set_attack (uint16_t attack) {
    if (attack == _last_attack) return;
    _last_attack = attack;
    _attack = calc_end_frame((attack<<2)+2);
  }
  void set_decay (uint16_t decay) {
    if (decay == _last_decay) return;
    _last_decay = decay;
    _decay = calc_end_frame((decay<<2)+2);
  }
  void set_sustain (uint16_t sustain) {
    if (sustain == _last_sustain) return;
    _last_sustain = sustain;
    _sustain = (sustain<<5);
  }
  void set_release (uint16_t release) {
    if (release == _last_release) return;
    _last_release = release;
    _release = calc_end_frame((release<<2)+2);
  }
  uint32_t calc_end_frame (uint32_t milliseconds) {
    return (milliseconds * _sample_rate) / 1000;
  }

  void modulate_vibrato (uint16_t vibrato) {
    volatile int32_t signedInput = vibrato;
    signedInput -= 0x7fff;
    _vibrato = static_cast<int8_t>(signedInput >> 8);
  }
  void modulate_tremelo (uint16_t tremelo) {
    _tremelo = tremelo; // should be a full range 16 bit uint
  }
  void modulate_vector (uint16_t vector_mod) {
    _vector_mod = vector_mod >> 6; // should be a full range 10 bit uint
  }
}





