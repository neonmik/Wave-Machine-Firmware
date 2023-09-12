#include "synth.h"
// #include <math.h>

#include "modulation.h"
#include "filter.h"
#include "fx.h"

namespace SYNTH {

  uint16_t _wave_shape;
  uint16_t _last_shape;

  uint16_t _wave_vector;
  uint16_t _vector_mod;

  uint16_t   _detune;
  uint16_t   _last_detune;

  uint32_t  _attack;
  uint32_t  _decay;
  uint32_t  _sustain;
  uint32_t  _release;
  // these are defaulted to outside the 10bit range so that when the EEPROM loads, it's always saved. (otherwise the attack/decay can be 0, which causes an overflow)
  uint16_t   _last_attack = 1024;
  uint16_t   _last_decay = 1024;
  uint16_t   _last_sustain = 1024;
  uint16_t   _last_release = 1024;

  int16_t   _vibrato;
  uint16_t  _tremelo;

  uint16_t pitchBend = 511;
  int16_t   _pitch_bend;
  uint16_t _last_pitch;

  uint8_t _octave = 0;
  uint16_t  _last_octave;

  uint16_t volume = 0xFFFF;
  // const int16_t sine_waveform[256] = {-32768,-32758,-32729,-32679,-32610,-32522,-32413,-32286,-32138,-31972,-31786,-31581,-31357,-31114,-30853,-30572,-30274,-29957,-29622,-29269,-28899,-28511,-28106,-27684,-27246,-26791,-26320,-25833,-25330,-24812,-24279,-23732,-23170,-22595,-22006,-21403,-20788,-20160,-19520,-18868,-18205,-17531,-16846,-16151,-15447,-14733,-14010,-13279,-12540,-11793,-11039,-10279,-9512,-8740,-7962,-7180,-6393,-5602,-4808,-4011,-3212,-2411,-1608,-804,0,804,1608,2411,3212,4011,4808,5602,6393,7180,7962,8740,9512,10279,11039,11793,12540,13279,14010,14733,15447,16151,16846,17531,18205,18868,19520,20160,20788,21403,22006,22595,23170,23732,24279,24812,25330,25833,26320,26791,27246,27684,28106,28511,28899,29269,29622,29957,30274,30572,30853,31114,31357,31581,31786,31972,32138,32286,32413,32522,32610,32679,32729,32758,32767,32758,32729,32679,32610,32522,32413,32286,32138,31972,31786,31581,31357,31114,30853,30572,30274,29957,29622,29269,28899,28511,28106,27684,27246,26791,26320,25833,25330,24812,24279,23732,23170,22595,22006,21403,20788,20160,19520,18868,18205,17531,16846,16151,15447,14733,14010,13279,12540,11793,11039,10279,9512,8740,7962,7180,6393,5602,4808,4011,3212,2411,1608,804,0,-804,-1608,-2411,-3212,-4011,-4808,-5602,-6393,-7180,-7962,-8740,-9512,-10279,-11039,-11793,-12540,-13279,-14010,-14733,-15447,-16151,-16846,-17531,-18205,-18868,-19520,-20160,-20788,-21403,-22006,-22595,-23170,-23732,-24279,-24812,-25330,-25833,-26320,-26791,-27246,-27684,-28106,-28511,-28899,-29269,-29622,-29957,-30274,-30572,-30853,-31114,-31357,-31581,-31786,-31972,-32138,-32286,-32413,-32522,-32610,-32679,-32729,-32758};

  Oscillators channels[POLYPHONY];


  void voice_on (uint8_t voice, uint8_t note) {
    if ((!channels[voice].isActive()) ||
        (!channels[voice].isGate())) {
      FILTER::voicesIncrease(); 
    }
    FILTER::trigger_attack();
    channels[voice].noteOn(note);
  }
  void voice_off (uint8_t voice) {
    if (channels[voice].isGate()) {
      FILTER::voicesDecrease(); 
    }
    FILTER::trigger_release();
    channels[voice].noteOff();
  }
  
  void Init () {
    MOD::Init();
    FILTER::Init();
  }

  
  bool is_audio_playing() {
    if(volume == 0) {
      return false;
    }

    bool any_channel_playing = false;
    for(int c = 0; c < POLYPHONY; c++) {
      if(channels[c].volume > 0 && channels[c].ADSR.isStopped()) {
        any_channel_playing = true;
      }
    }

    return any_channel_playing;
  }

  // uint16_t softStart () {
  //   ++_soft_start_index;

  //   if (_soft_start_index >= 2) {
  //     _soft_start_index = 0;
  //     _soft_start_sample += 1;
  //     if (_soft_start_sample > 0) {
  //       _soft_start_sample = 0;
  //     }
  //   }

  //   return (_soft_start_sample - INT16_MIN)>>4;
  // }

  uint16_t process() {
    
    if (_soft_start) {
      ++_soft_start_index;
      if (_soft_start_index >= 2) {
        _soft_start_index = 0;
        _soft_start_sample += 1;
        if (_soft_start_sample >= 0) {
          _soft_start_sample = 0;
          _soft_start = false;
        }
      }
      return (_soft_start_sample - INT16_MIN)>>4;
    } 

    else {
      int32_t outputSample = 0;  // used to combine channel output
      
      MOD::Update();

      // implemented this here so that it's set for the whole sample run...
      uint16_t vector = (_wave_shape + (_wave_vector + _vector_mod));
      uint16_t output_volume = (volume - _tremelo);
      
      for(int c = 0; c < POLYPHONY; c++) {

        auto &channel = channels[c];
        // check if any waveforms are active for this channel
        if(channel.isActive()) {
          
          channel.calcIncrement();
          // increment the waveform position counter.
          channel.phaseAccumulator += channel.phaseIncrement;


          //this is where vibrato is added... has to be here and not in the pitch scale as it would be lopsided due to logarithmic nature of freqencies.
          channel.phaseAccumulator += _vibrato;

          channel.ADSR.Update();
          if (channel.isActive() && channel.ADSR.isStopped()) {
            channel.noteStopped();
            QUEUE::release_send(c);
            continue; // save processing this channel any futher
          }

          int32_t channelSample = 0;

          // pretty sure I can remove this if not using the other wave types. but also increasing it to 0xffff0 makes it large enough to not interupt my scaling.
          channel.phaseAccumulator &= (0xffff0);
          
          channelSample += getWavetable(channel.phaseAccumulator, vector); // >> Q_SCALING_FACTOR removed for interpolationg wavetable
          // channelSample += getWavetable((channel.phaseAccumulator >> 1), vector); // Sub oscillator test
          
          // apply ADSR
          channelSample = (int32_t(channelSample) * int32_t(channel.ADSR.get())) >> 16;

          // apply channel volume
          channelSample = (int32_t(channelSample) * int32_t(channel.volume)) >> 16;

          outputSample += channelSample;
        }
      }


      outputSample = (int32_t(outputSample >> 3) * int32_t(output_volume)) >> 16; // needs to shift by 19 as to deal with possibly 8 voices... it would only need to be shifted by 16 if the output was 1* 16 bit, not 8*16 bit

      // was meant to introduce lower sample rate filter, but makes a bit crushed effect...
      // m++;
      // m &= 0x7;
      // if (m == 0) FILTER::process(outputSample);
      
      // Filter
      FILTER::process(outputSample);

      // Soft clipping
      FX::SOFTCLIP::process(outputSample);

      // Hard clipping to 16-bit
      FX::HARDCLIP::process(outputSample);
      
      // move sample to unsigned space, and then shift it down 4 to make it 12 bit for the dac
      return (outputSample - INT16_MIN)>>4;
    }

  }

  void set_waveshape (uint16_t shape) {
    uint16_t temp = ((shape >> 6) << 8); // the double bit shifts here are to loose precision.
    if (temp == _last_shape) return;
    _last_shape = temp;
    _wave_shape = temp;
  }
  void set_wavevector (uint16_t vector) {
    _wave_vector = vector;
  }
  void set_octave (uint16_t octave) {
    if (octave == _last_octave) return;
    _octave = (octave>>8);
    _last_octave = octave;
  }
  void set_pitch_scale (uint16_t bend) {
    if (bend == _last_pitch) return;
    pitchBend = pitch_log(bend);
    _last_pitch = bend;
  }

  void set_attack (uint16_t attack) {
    if (attack == _last_attack) return;
    _last_attack = attack;
    // _attack = (attack << 2) + 2;
    _attack = calc_end_frame((attack<<2)+2);
  }
  void set_decay (uint16_t decay) {
    if (decay == _last_decay) return;
    _last_decay = decay;
    // _decay = (decay << 2) + 2;
    _decay = calc_end_frame((decay<<2)+2);
  }
  void set_sustain (uint16_t sustain) {
    if (sustain == _last_sustain) return;
    _last_sustain = sustain;
    _sustain = (sustain << 6);
  }
  void set_release (uint16_t release) {
    if (release == _last_release) return;
    _last_release = release;
    // _release = (release << 2) + 2;
    _release = calc_end_frame((release<<2)+2);
  }
  uint32_t calc_end_frame (uint32_t milliseconds) {
    return (milliseconds * SAMPLE_RATE) / 1000;
  }

  void modulate_vibrato (uint16_t vibrato) {
    volatile int32_t signedInput = vibrato;
    signedInput -= 0x7fff;  
    _vibrato = static_cast<int8_t>(signedInput >> 8);

    // doesnt work may need a different table
    // int16_t temp = pitch_log(vibrato >> 6); // shift the u16 input to u10, then map it to pitch (256-1023)
    // temp -= 0x200;
    // _vibrato = pitch_log(vibrato >> 6);
  }
  void modulate_tremelo (uint16_t tremelo) {
    _tremelo = tremelo;
  }
  void modulate_vector (uint16_t vector_mod) {
    _vector_mod = vector_mod >> 6;
  }

}



    