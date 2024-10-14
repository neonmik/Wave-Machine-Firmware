#include "synth.h"

#include "modulation.h"

#include "fx.h"

namespace SYNTH {

  void voiceOn (uint8_t voice, uint8_t note) {
    if ((!synth.voices[voice].isActive()) ||
        (!synth.voices[voice].isGate())) {
      // FILTER::voicesIncrease();
      MOD::voicesIncrease();
    }
    FILTER::triggerAttack(voice);
    MOD::triggerAttack();
    synth.voices[voice].noteOn(note);
  }
  void voiceOff (uint8_t voice) {
    if (synth.voices[voice].isGate()) {
      // FILTER::voicesDecrease(); 
      MOD::voicesDecrease();
    }
    FILTER::triggerRelease(voice);
    MOD::triggerRelease();
    synth.voices[voice].noteOff();
  }
  
  void init () {
    MOD::init();
    FILTER::init();
  }

  uint16_t process() {
    
    synth.sample = 0; 
    
    MOD::update();
    synthParameters.updateWaveshape();
    
    for(int c = 0; c < POLYPHONY; c++) {
      synth.sample += synth.voices[c].process();
    }

    synth.sample = synth.sample >> 2;
  
    
    FX::SOFTCLIP::process(synth.sample); // Soft clipping to 16-bit
    
    FX::HARDCLIP::process16(synth.sample); // Hard clipping to 16-bit
    
    // move sample to unsigned space, and then shift it down 4 to make it 12 bit for the dac
    return (synth.sample - INT16_MIN)>>4;
  }

  void calculateIncrements(void) {
    if (recalculateIncrement) {
      for(int i = 0; i < POLYPHONY; i++) {
        synth.voices[i].refreshIncrement = true;
        synth.voices[i].updateIncrement();
      }
      recalculateIncrement = false;
    } else {
      for(int i = 0; i < POLYPHONY; i++) {
        synth.voices[i].updateIncrement();
      }
    }
  }

  void setWaveShape (uint16_t input) {
    synthParameters.oscillator1.waveShape = ((input >> 6) << 8);
  }
  void setWaveVector (uint16_t input) {
    synthParameters.oscillator1.waveVector = input;
  }
  void setOctave (uint16_t input) {
    // old implementation - also check synth increment code
    // synthParameters.oscillator1.octave = (input >> 8);

    // this will still need looking at to improve values jumping at boarders
    synthParameters.oscillator1.octave = getOctave(input); // shift is here to reduce noise bouncing the values - I've also reduced the table size.
    recalculateIncrement = true;
  }
  void setPitchBend (uint16_t input) {
    synthParameters.oscillator1.pitchBend = logarithmicPitch(input);
    recalculateIncrement = true;
  }

  void setAttack (uint16_t input) {
    synthParameters.envelopeControls.setAttack(input);
  }
  void setDecay (uint16_t input) {
    synthParameters.envelopeControls.setDecay(input);
  }
  void setSustain (uint16_t input) {
    synthParameters.envelopeControls.setSustain(input);
  }
  void setRelease (uint16_t input) {
    synthParameters.envelopeControls.setRelease(input);
  }

  void modulateVibrato (uint16_t input) {
    volatile int32_t signedInput = input;
    signedInput -= 0x7fff;  
    synthParameters.modVibrato = static_cast<int16_t>(signedInput >> 5);
  }
  void modulateTremelo (uint16_t input) {
    synthParameters.modTremelo =  input;
  }
  void modulateVector (uint16_t input) {
    synthParameters.modVector = input >> 6;
  }

  void setSub (uint16_t input) {
    synthParameters.oscillator2.level = (input << 6);
  }
  void setNoise (uint16_t input) {
    synthParameters.oscillatorN.level = (input << 6);
  }

  void setDetune (uint16_t input) {
    synthParameters.detune = logarithmicPitch(input); 

    recalculateIncrement = true;
  }
  void setOsc2Wave (uint16_t input) {
    synthParameters.oscillator2.waveShape = ((input >> 6) << 8);
  }
}



    