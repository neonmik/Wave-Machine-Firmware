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
  uint16_t   lastAttack = 1024;
  uint16_t   lastDecay = 1024;
  uint16_t   lastSustain = 1024;
  uint16_t   lastRelease = 1024;

  int16_t   _vibrato;
  uint16_t  _tremelo;

  uint16_t pitchBend = 511;
  int16_t   _pitch_bend;
  uint16_t _last_pitch;

  uint8_t _octave = 0;
  uint16_t  _last_octave;

  uint16_t volume = 0xFFFF;

  Oscillators channels[POLYPHONY];


  void voiceOn (uint8_t voice, uint8_t note) {
    if ((!channels[voice].isActive()) ||
        (!channels[voice].isGate())) {
      FILTER::voicesIncrease(); 
    }
    FILTER::triggerAttack();
    channels[voice].noteOn(note);
  }
  void voiceOff (uint8_t voice) {
    if (channels[voice].isGate()) {
      FILTER::voicesDecrease(); 
    }
    FILTER::triggerRelease();
    channels[voice].noteOff();
  }
  
  void init () {
    MOD::init();
    FILTER::init();
  }

  // uint16_t softStart () {
  //   ++softStartIndex;

  //   if (softStartIndex >= 2) {
  //     softStartIndex = 0;
  //     softStartSample += 1;
  //     if (softStartSample > 0) {
  //       softStartSample = 0;
  //     }
  //   }

  //   return (softStartSample - INT16_MIN)>>4;
  // }

  uint16_t process() {
    
    if (playSoftStart) {
      ++softStartIndex;
      if (softStartIndex >= 2) {
        softStartIndex = 0;
        softStartSample += 1;
        if (softStartSample >= 0) {
          softStartSample = 0;
          playSoftStart = false;
        }
      }
      return (softStartSample - INT16_MIN)>>4;
    } 

    else {
      int32_t outputSample = 0;  // used to combine channel output
      
      MOD::update();

      // implemented this here so that it's set for the whole sample run...
      uint16_t vector = (_wave_shape + (_wave_vector + _vector_mod));
      uint16_t outputVolume = (volume - _tremelo);
      
      for(int c = 0; c < POLYPHONY; c++) {

        auto &channel = channels[c];
        // check if any waveforms are active for this channel
        if(channel.isActive()) {
          
          channel.calcIncrement();
          // increment the waveform position counter.
          channel.phaseAccumulator += channel.phaseIncrement;


          //this is where vibrato is added... has to be here and not in the pitch scale as it would be lopsided due to logarithmic nature of freqencies.
          channel.phaseAccumulator += _vibrato;

          channel.ADSR.update();
          if (channel.isActive() && channel.ADSR.isStopped()) {
            channel.noteStopped();
            QUEUE::releaseSend(c);
            continue; // save processing this channel any futher
          }

          int32_t channelSample = 0;

          // 0xffff0 makes it large enough to not interupt my scaling, anything more than like 0xfffff starts to go mad... Seems to mess with the waveforms
          channel.phaseAccumulator &= (0xffff0);
          
          channelSample += getWavetable(channel.phaseAccumulator, vector); // >> Q_SCALING_FACTOR removed for interpolationg wavetable
          
          // Prototype oscillator modes:

          // Sub Mode
          // channelSample += getWavetable((channel.phaseAccumulator >> 2), 0); // Sub Sinewave oscillator test - currently doesn't work, >> 1 creates a half wave, >> 2 creates a quarter wave
          // channelSample /= 2;

          // Noise Mode
          // channelSample = RANDOM::get() >> 4; // returns noise... currently for testing how random the output is.

          // apply ADSR
          channelSample = (int32_t(channelSample) * int32_t(channel.ADSR.get())) >> 16;

          // apply channel volume
          channelSample = (int32_t(channelSample) * int32_t(channel.volume)) >> 16;

          outputSample += channelSample;
        }
      }


      outputSample = (int32_t(outputSample >> 3) * int32_t(outputVolume)) >> 16; // needs to shift by 19 as to deal with possibly 8 voices... it would only need to be shifted by 16 if the output was 1* 16 bit, not 8*16 bit

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

  void setWaveshape (uint16_t shape) {
    uint16_t temp = ((shape >> 6) << 8); // the double bit shifts here are to loose precision.
    if (temp == _last_shape) return;
    _last_shape = temp;
    _wave_shape = temp;
  }
  void setWavevector (uint16_t vector) {
    _wave_vector = vector;
  }
  void setOctave (uint16_t octave) {
    if (octave == _last_octave) return;
    _octave = (octave>>8);
    _last_octave = octave;
  }
  void setPitchBend (uint16_t bend) {
    if (bend == _last_pitch) return;
    pitchBend = logarithmicPitch(bend);
    _last_pitch = bend;
  }

  void setAttack (uint16_t attack) {
    if (attack == lastAttack) return;
    lastAttack = attack;
    // _attack = (attack << 2) + 2;
    _attack = calculateEndFrame(attack << 2);
  }
  void setDecay (uint16_t decay) {
    if (decay == lastDecay) return;
    lastDecay = decay;
    // _decay = (decay << 2) + 2;
    _decay = calculateEndFrame(decay << 2);
  }
  void setSustain (uint16_t sustain) {
    if (sustain == lastSustain) return;
    lastSustain = sustain;
    _sustain = (sustain << 6);
  }
  void setRelease (uint16_t release) {
    if (release == lastRelease) return;
    lastRelease = release;
    // _release = (release << 2) + 2;
    _release = calculateEndFrame(release << 2);
  }
  uint32_t calculateEndFrame (uint32_t milliseconds) {
    return ((milliseconds + 1) * SAMPLE_RATE) / 1000; // + 1 so that it can never be 0, as it just creates noise.
  }

  void modulateVibrato (uint16_t vibrato) {
    volatile int32_t signedInput = vibrato;
    signedInput -= 0x7fff;  
    _vibrato = static_cast<int8_t>(signedInput >> 8);
  }
  void modulateTremelo (uint16_t tremelo) {
    _tremelo = tremelo;
  }
  void modulateVector (uint16_t vector_mod) {
    _vector_mod = vector_mod >> 6;
  }

}



    