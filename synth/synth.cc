#include "synth.h"
#include <math.h>


namespace SYNTH {
  uint16_t waveforms = Waveform::WAVETABLE; // | Waveform::TRIANGLE;      // bitmask for enabled waveforms (see AudioWaveform enum for values)

  uint16_t wave;
  uint16_t wave_vector;
  uint16_t vector_mod;

  uint16_t  attack_ms   = 10;      // attack period - moved to global as it's not needed per voice for this implementation.
  uint16_t  decay_ms    = 10;      // decay period
  uint16_t  sustain     = 0x6fff;   // sustain volume
  uint16_t  release_ms  = 500;      // release period

  int16_t   vibrato;
  uint16_t  tremelo;

  uint16_t pitch_scale = 512;

  uint8_t octave = 0;

  // bool      filter_enable = false;
  // uint16_t  filter_cutoff_frequency = 400 ;

  // float filter_epow = 1 - expf(-(1.0f / 22050.0f) * 2.0f * pi * int32_t(filter_cutoff_frequency));

  uint16_t volume = 0xffff;
  const int16_t sine_waveform[256] = {-32768,-32758,-32729,-32679,-32610,-32522,-32413,-32286,-32138,-31972,-31786,-31581,-31357,-31114,-30853,-30572,-30274,-29957,-29622,-29269,-28899,-28511,-28106,-27684,-27246,-26791,-26320,-25833,-25330,-24812,-24279,-23732,-23170,-22595,-22006,-21403,-20788,-20160,-19520,-18868,-18205,-17531,-16846,-16151,-15447,-14733,-14010,-13279,-12540,-11793,-11039,-10279,-9512,-8740,-7962,-7180,-6393,-5602,-4808,-4011,-3212,-2411,-1608,-804,0,804,1608,2411,3212,4011,4808,5602,6393,7180,7962,8740,9512,10279,11039,11793,12540,13279,14010,14733,15447,16151,16846,17531,18205,18868,19520,20160,20788,21403,22006,22595,23170,23732,24279,24812,25330,25833,26320,26791,27246,27684,28106,28511,28899,29269,29622,29957,30274,30572,30853,31114,31357,31581,31786,31972,32138,32286,32413,32522,32610,32679,32729,32758,32767,32758,32729,32679,32610,32522,32413,32286,32138,31972,31786,31581,31357,31114,30853,30572,30274,29957,29622,29269,28899,28511,28106,27684,27246,26791,26320,25833,25330,24812,24279,23732,23170,22595,22006,21403,20788,20160,19520,18868,18205,17531,16846,16151,15447,14733,14010,13279,12540,11793,11039,10279,9512,8740,7962,7180,6393,5602,4808,4011,3212,2411,1608,804,0,-804,-1608,-2411,-3212,-4011,-4808,-5602,-6393,-7180,-7962,-8740,-9512,-10279,-11039,-11793,-12540,-13279,-14010,-14733,-15447,-16151,-16846,-17531,-18205,-18868,-19520,-20160,-20788,-21403,-22006,-22595,-23170,-23732,-24279,-24812,-25330,-25833,-26320,-26791,-27246,-27684,-28106,-28511,-28899,-29269,-29622,-29957,-30274,-30572,-30853,-31114,-31357,-31581,-31786,-31972,-32138,-32286,-32413,-32522,-32610,-32679,-32729,-32758};

  Oscillators channels[MAX_VOICES];

  uint32_t prng_xorshift_state = 0x32B71700;

  uint32_t prng_xorshift_next() {
    uint32_t x = prng_xorshift_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    prng_xorshift_state = x;
    return x;
  }

  int32_t prng_normal() {
    // rough approximation of a normal distribution
    uint32_t r0 = prng_xorshift_next();
    uint32_t r1 = prng_xorshift_next();
    uint32_t n = ((r0 & 0xffff) + (r1 & 0xffff) + (r0 >> 16) + (r1 >> 16)) / 2;
    return n - 0xffff;
  }

  uint32_t sample_rate;
  
  void init (uint32_t _sample_rate) {
    sample_rate = _sample_rate;
  }

  
  bool is_audio_playing() {
    if(volume == 0) {
      return false;
    }

    bool any_channel_playing = false;
    for(int c = 0; c < MAX_VOICES; c++) {
      if(channels[c].volume > 0 && channels[c].adsr_phase != ADSRPhase::OFF) {
        any_channel_playing = true;
      }
    }

    return any_channel_playing;
  }

  uint16_t get_audio_frame() {
    int32_t sample = 0;  // used to combine channel output
    int16_t clipped_sample = 0;

    // implemented this here so that it's set for the whole sample run...
    uint16_t vector = (wave + (wave_vector + vector_mod));
    int32_t output_volume = (volume - tremelo);

    
    
    for(int c = 0; c < MAX_VOICES; c++) {

      auto &channel = channels[c];

      
      // increment the waveform position counter. this provides an
      // Q16 fixed point value representing how far through
      // the current waveform we are
      channel.waveform_offset += (((((channel.frequency * pitch_scale)>>9) << octave) * 256) << 8) / sample_rate;

      //this is where vibrato is added... has to be here and not in the pitch scale as it would be lopsided due to logarithmic nature of freqencies.
      channel.waveform_offset += vibrato;

      if(channel.adsr_phase == ADSRPhase::OFF) {
        continue;
      }

      if ((channel.adsr_frame >= channel.adsr_end_frame) && (channel.adsr_phase != ADSRPhase::SUSTAIN)) {
        switch (channel.adsr_phase) {
          case ADSRPhase::ATTACK:
            channel.trigger_decay();
            break;
          case ADSRPhase::DECAY:
            channel.trigger_sustain();
            break;
          case ADSRPhase::RELEASE:
            channel.off();
            break;
          default:
            break;
        }
      }

      
      channel.adsr += channel.adsr_step;
      channel.adsr_frame++;

      if(channel.waveform_offset & 0x10000) {
        // if the waveform offset overflows then generate a new
        // random noise sample
        channel.noise = prng_normal();
      }

      channel.waveform_offset &= 0xffff;

      // check if any waveforms are active for this channel
      if(channel.is_active) {
        uint8_t waveform_count = 0;
        int32_t channel_sample = 0;


        if(waveforms & Waveform::NOISE) {
          channel_sample += channel.noise;
          waveform_count++;
        }

        if(waveforms & Waveform::SAW) {
          channel_sample += (int32_t)channel.waveform_offset - 0x7fff;
          waveform_count++;
        }

        // creates a triangle wave of ^
        if (waveforms & Waveform::TRIANGLE) {
          if (channel.waveform_offset < 0x7fff) { // initial quarter up slope
            channel_sample += int32_t(channel.waveform_offset * 2) - int32_t(0x7fff);
          }
          else { // final quarter up slope
            channel_sample += int32_t(0x7fff) - ((int32_t(channel.waveform_offset) - int32_t(0x7fff)) * 2);
          }
          waveform_count++;
        }

        if (waveforms & Waveform::SQUARE) {
          channel_sample += (channel.waveform_offset < channel.pulse_width) ? 0x7fff : -0x7fff;
          waveform_count++;
        }
        
        if(waveforms & Waveform::SINE) {
          // the sine_waveform sample contains 256 samples in
          // total so we'll just use the most significant bits
          // of the current waveform position to index into it
          channel_sample += sine_waveform[(channel.waveform_offset >> 8)];
          waveform_count++;
          
        }

        if(waveforms & Waveform::WAVETABLE) {

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
        channel_sample = (int64_t(channel_sample) * int32_t(channel.adsr >> 8)) >> 16;

        // apply channel volume
        channel_sample = (int64_t(channel_sample) * int32_t(channel.volume)) >> 16;

        // apply channel filter
        // if (filter_enable) {
        //   channel_sample += (channel_sample - channel.filter_last_sample) * filter_epow;
        // }

        // channel.filter_last_sample = channel_sample;
        

        sample += channel_sample;
      }
    }

    sample = (int64_t(sample) * output_volume) >> 16;

    //attempt at soft clipping - doesnt work
    // sample = ((sample + (sample>>1))-10) * sample - ((sample>>1)-10) * sample * sample * sample;

    // clip result to 16-bit
    sample = sample <= -0x8000 ? -0x8000 : (sample > 0x7fff ? 0x7fff : sample);
    
    

    return (sample+32767)>>4;
  }
}