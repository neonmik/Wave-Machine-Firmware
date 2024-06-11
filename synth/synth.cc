#include "synth.h"

#include "modulation.h"
#include "filter.h"
#include "fx.h"

namespace SYNTH {

  void voiceOn (uint8_t voice, uint8_t note) {
    if ((!synth.voices[voice].isActive()) ||
        (!synth.voices[voice].isGate())) {
      FILTER::voicesIncrease();
      MOD::voicesIncrease();
    }
    FILTER::triggerAttack();
    MOD::triggerAttack();
    synth.voices[voice].noteOn(note);
  }
  void voiceOff (uint8_t voice) {
    if (synth.voices[voice].isGate()) {
      FILTER::voicesDecrease(); 
      MOD::voicesDecrease();
    }
    FILTER::triggerRelease();
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
    synth.updateWaveshape();
    
    for(int c = 0; c < POLYPHONY; c++) {
      synth.sample += synth.voices[c].process(voice_index);
    }

    synth.sample = synth.sample >> 2;
    
    FILTER::process(synth.sample); // Filter
    
    FX::SOFTCLIP::process(synth.sample); // Soft clipping to 16-bit
    
    FX::HARDCLIP::process16(synth.sample); // Hard clipping to 16-bit
    
    // move sample to unsigned space, and then shift it down 4 to make it 12 bit for the dac
    return (synth.sample - INT16_MIN)>>4;
  }

  void calculateIncrements(void) {
    for(int i = 0; i < POLYPHONY; i++) {
      synth.voices[i].updateIncrement();
    }
  }

  void setWaveShape (uint16_t input) {
    synth.synthParameters.oscillator1.waveShape = ((input >> 6) << 8);
  }
  void setWaveVector (uint16_t input) {
    synth.synthParameters.oscillator1.waveVector = input;
  }
  void setOctave (uint16_t input) {
    synth.synthParameters.oscillator1.octave = (input >> 8);
  }
  void setPitchBend (uint16_t input) {
    synth.synthParameters.oscillator1.pitchBend = logarithmicPitch(input);
  }

  void setAttack (uint16_t input) {
    synth.synthParameters.envelopeControls.setAttack(input);
  }
  void setDecay (uint16_t input) {
    synth.synthParameters.envelopeControls.setDecay(input);
  }
  void setSustain (uint16_t input) {
    synth.synthParameters.envelopeControls.setSustain(input);
  }
  void setRelease (uint16_t input) {
    synth.synthParameters.envelopeControls.setRelease(input);
  }

  void modulateVibrato (uint16_t input) {
    volatile int32_t signedInput = input;
    signedInput -= 0x7fff;  
    synth.synthParameters.modVibrato = static_cast<int16_t>(signedInput >> 5);
  }
  void modulateTremelo (uint16_t input) {
    synth.synthParameters.modTremelo =  input;
  }
  void modulateVector (uint16_t input) {
    synth.synthParameters.modVector = input >> 6;
  }

  void setSub (uint16_t input) {
    synth.synthParameters.oscillator2.level = (input << 6);
  }
  void setNoise (uint16_t input) {
    synth.synthParameters.oscillatorN.level = (input << 6);
  }

  void setDetune (uint16_t input) {
    uint16_t temp = logPotentiometer(input);

    if (temp == 0) synth.synthParameters.detune = 0;
    else synth.synthParameters.detune = 1024 - temp;
  }
  void setOsc2Wave (uint16_t input) {
    synth.synthParameters.oscillator2.waveShape = ((input >> 6) << 8);
  }
}



    