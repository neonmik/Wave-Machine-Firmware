#include "synth.h"
#include <math.h>

uint16_t pitch_scale;

namespace synth {

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

  uint16_t volume = 0xffff;
  const int16_t sine_waveform[256] = {-32768,-32758,-32729,-32679,-32610,-32522,-32413,-32286,-32138,-31972,-31786,-31581,-31357,-31114,-30853,-30572,-30274,-29957,-29622,-29269,-28899,-28511,-28106,-27684,-27246,-26791,-26320,-25833,-25330,-24812,-24279,-23732,-23170,-22595,-22006,-21403,-20788,-20160,-19520,-18868,-18205,-17531,-16846,-16151,-15447,-14733,-14010,-13279,-12540,-11793,-11039,-10279,-9512,-8740,-7962,-7180,-6393,-5602,-4808,-4011,-3212,-2411,-1608,-804,0,804,1608,2411,3212,4011,4808,5602,6393,7180,7962,8740,9512,10279,11039,11793,12540,13279,14010,14733,15447,16151,16846,17531,18205,18868,19520,20160,20788,21403,22006,22595,23170,23732,24279,24812,25330,25833,26320,26791,27246,27684,28106,28511,28899,29269,29622,29957,30274,30572,30853,31114,31357,31581,31786,31972,32138,32286,32413,32522,32610,32679,32729,32758,32767,32758,32729,32679,32610,32522,32413,32286,32138,31972,31786,31581,31357,31114,30853,30572,30274,29957,29622,29269,28899,28511,28106,27684,27246,26791,26320,25833,25330,24812,24279,23732,23170,22595,22006,21403,20788,20160,19520,18868,18205,17531,16846,16151,15447,14733,14010,13279,12540,11793,11039,10279,9512,8740,7962,7180,6393,5602,4808,4011,3212,2411,1608,804,0,-804,-1608,-2411,-3212,-4011,-4808,-5602,-6393,-7180,-7962,-8740,-9512,-10279,-11039,-11793,-12540,-13279,-14010,-14733,-15447,-16151,-16846,-17531,-18205,-18868,-19520,-20160,-20788,-21403,-22006,-22595,-23170,-23732,-24279,-24812,-25330,-25833,-26320,-26791,-27246,-27684,-28106,-28511,-28899,-29269,-29622,-29957,-30274,-30572,-30853,-31114,-31357,-31581,-31786,-31972,-32138,-32286,-32413,-32522,-32610,-32679,-32729,-32758};

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

  int16_t get_audio_frame() {
    int32_t sample = 0;  // used to combine channel output
    int16_t clipped_sample = 0;
    
    for(int c = 0; c < MAX_VOICES; c++) {

      auto &channel = channels[c];

      
      // increment the waveform position counter. this provides an
      // Q16 fixed point value representing how far through
      // the current waveform we are

      // if we put 440Hz in here
      channel.waveform_offset += ((((channel.frequency * pitch_scale)>>9) * 256) << 8) / sample_rate;

      // it comes out at 653 - all the numbers after the decimal get removed, the actual result is 653.873923

      // this skips the channel (voice/oscillator) entirely if it's off, goes to the next for loop number
      if(channel.adsr_phase == ADSRPhase::OFF) {
        continue;
      }


      if ((channel.adsr_frame >= channel.adsr_end_frame) && (channel.adsr_phase != ADSRPhase::SUSTAIN)) {
        switch (channel.adsr_phase) {
          case ADSRPhase::ATTACK:
            channel.trigger_decay();
            // printf("Trigged DECAY [%d] \n", c);
            break;
          case ADSRPhase::DECAY:
            channel.trigger_sustain();
            // printf("Trigged SUSTAIN [%d] \n", c);
            break;
          case ADSRPhase::RELEASE:
            // printf("Trigged OFF [%d] \n", c);
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


        if(channel.waveforms & Waveform::NOISE) {
          channel_sample += channel.noise;
          waveform_count++;
        }

        if(channel.waveforms & Waveform::SAW) {
          channel_sample += (int32_t)channel.waveform_offset - 0x7fff;
          waveform_count++;
        }

        // creates a triangle wave of ^
        if (channel.waveforms & Waveform::TRIANGLE) {
          if (channel.waveform_offset < 0x7fff) { // initial quarter up slope
            channel_sample += int32_t(channel.waveform_offset * 2) - int32_t(0x7fff);
          }
          else { // final quarter up slope
            channel_sample += int32_t(0x7fff) - ((int32_t(channel.waveform_offset) - int32_t(0x7fff)) * 2);
          }
          waveform_count++;
        }

        if (channel.waveforms & Waveform::SQUARE) {
          channel_sample += (channel.waveform_offset < channel.pulse_width) ? 0x7fff : -0x7fff;
          waveform_count++;
        }

        if(channel.waveforms & Waveform::SINE) {
          // the sine_waveform sample contains 256 samples in
          // total so we'll just use the most significant bits
          // of the current waveform position to index into it
          channel_sample += sine_waveform[channel.waveform_offset >> 8];
          waveform_count++;
        }

        if(channel.waveforms & Waveform::WAVE) {
          channel_sample += channel.wave_buffer[channel.wave_buf_pos];
          if (++channel.wave_buf_pos == 64) {
            channel.wave_buf_pos = 0;
            if(channel.wave_buffer_callback)
                channel.wave_buffer_callback(channel);
          }
          waveform_count++;
        }

        channel_sample = channel_sample / waveform_count;
        // if (c==0) printf("PRE ADSR SAMPLE: %.5d [%d] \n", channel_sample, c);

        channel_sample = (int64_t(channel_sample) * int32_t(channel.adsr >> 8)) >> 16;
        // if (c==0) printf("POST ADSR SAMPLE: %.5d [%d] \n", channel_sample, c);

        // apply channel volume
        channel_sample = (int64_t(channel_sample) * int32_t(channel.volume)) >> 16;
        // if (c==0) printf("POST VOLUME SAMPLE: %.5d [%d] \n", channel_sample, c);
        
        // combine channel sample into the final sample
        // if (c==0) printf("channel_sample = %.5d  | ", channel_sample, c);
        sample += channel_sample;

        // not clipped here with full volume at channel settings and main settings
        // if (c==0) printf(" sample = %.5d\n", sample, c);
      }
    }
    // printf("PRE MASTER VOLUME SAMPLE: %.5d \n", sample);
    sample = (int64_t(sample) * int32_t(volume)) >> 16;
    //  printf("POST MASTER VOLUME SAMPLE: %.5d \n", sample);
    // clip result to 16-bit
    sample = sample <= -0x8000 ? -0x8000 : (sample > 0x7fff ? 0x7fff : sample);
    // sample = ((sample + (sample>>1))-10) * sample - ((sample>>1)-10) * sample * sample * sample;
    // printf("%.5d \n", sample);
    return sample;
  }
}
