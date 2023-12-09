#include "synth.h"

#include "modulation.h"
#include "filter.h"
#include "fx.h"

namespace SYNTH {

  Oscillators channels[POLYPHONY];


  void voiceOn (uint8_t voice, uint8_t note) {
    if ((!channels[voice].isActive()) ||
        (!channels[voice].isGate())) {
      FILTER::voicesIncrease();
      MOD::voicesIncrease();
    }
    FILTER::triggerAttack();
    MOD::triggerAttack();
    channels[voice].noteOn(note);
  }
  void voiceOff (uint8_t voice) {
    if (channels[voice].isGate()) {
      FILTER::voicesDecrease(); 
      MOD::voicesDecrease();
    }
    FILTER::triggerRelease();
    MOD::triggerRelease();
    channels[voice].noteOff();
  }
  
  void init () {
    MOD::init();
    FILTER::init();
  }

  uint16_t process() {
    int32_t outputSample = 0;  // used to combine channel output
    
    MOD::update();

    // implemented this here so that it's set for the whole sample run...
    uint16_t waveMod = (currentWaveVector + modVector);
    uint16_t waveOffset = (currentWaveShape + waveMod);
    uint16_t wave2Offset = (osc2Wave + waveMod);
    uint16_t outputVolume = (volume - modTremelo);
    
    for(int c = 0; c < POLYPHONY; c++) {

      auto &channel = channels[c];
      // check if any waveforms are active for this channel
      if(channel.isActive()) {
        
        channel.calcIncrement();
        // increment the waveform position counter.
        channel.phaseAccumulator += channel.phaseIncrement;


        //this is where modVibrato is added... 
        channel.phaseAccumulator += modVibrato;

        channel.ampEnvelope.update();
        if (channel.isActive() && channel.ampEnvelope.isStopped()) {
          channel.noteStopped();
          QUEUE::releaseSend(c);
          continue; // save processing this channel any futher
        }

        int32_t channelSample = 0;
        
        channelSample += getWavetableInterpolated(channel.phaseAccumulator, waveOffset); // >> Q_SCALING_FACTOR removed for interpolationg wavetable
        
        // Prototype oscillator modes:

        // Second Oscillator - Sub or Detuned oscillator
        // - Set to 0, this oscilaltor will be a -1 oct Sub
        // - Set to any other value, this will be a detuned 

        uint32_t offset;
        if (currentDetune == 0) {
          offset = 0;
          channelSample += (getWavetableInterpolated((channel.phaseAccumulator >> 1), wave2Offset) * subLevel) >> 10; // Sub Sinewave oscillator
        }
        else {
          offset =  channel.phaseAccumulator / currentDetune;
          channelSample += (getWavetableInterpolated(channel.phaseAccumulator + offset, wave2Offset) * subLevel) >> 10; // Second oscillator test
        }

        channelSample += ((RANDOM::getSignal() >> 2) * noiseLevel) >> 10;

        channelSample /= 3;


        // apply Amp envelope
        channelSample = (int32_t(channelSample) * int32_t(channel.ampEnvelope.get())) >> 16;

        // apply channel volume - should be velocity eventually
        channelSample = (int32_t(channelSample) * int32_t(channel.volume)) >> 16;

        outputSample += channelSample;
      }
    }


    outputSample = (int32_t(outputSample >> 2) * int32_t(outputVolume)) >> 16; // needs to shift by 18 as to deal with possibly 8 voices of 3 osc... it would only need to be shifted by 16 if the output was 1* 16 bit, not 8*16 bit
    
    // Filter
    FILTER::process(outputSample);

    // Soft clipping to 16-bit
    FX::SOFTCLIP::process(outputSample);

    // Hard clipping to 16-bit
    FX::HARDCLIP::process16(outputSample);
    
    // move sample to unsigned space, and then shift it down 4 to make it 12 bit for the dac
    return (outputSample - INT16_MIN)>>4;
  }

  void setWaveShape (uint16_t input) {
    currentWaveShape = ((input >> 6) << 8);
  }
  void setWaveVector (uint16_t input) {
    currentWaveVector = input;
  }
  void setOctave (uint16_t input) {
    currentOctave = input >> 8;
  }
  void setPitchBend (uint16_t input) {
    currentPitchBend = logarithmicPitch(input);
  }

  void setAttack (uint16_t input) {
    envelopeControls.setAttack(input);
  }
  void setDecay (uint16_t input) {
    envelopeControls.setDecay(input);
  }
  void setSustain (uint16_t input) {
    envelopeControls.setSustain(input);
  }
  void setRelease (uint16_t input) {
    envelopeControls.setRelease(input);
  }

  void modulateVibrato (uint16_t input) {
    volatile int32_t signedInput = input;
    signedInput -= 0x7fff;  
    modVibrato = static_cast<int8_t>(signedInput >> 8);
  }
  void modulateTremelo (uint16_t input) {
    modTremelo = input;
  }
  void modulateVector (uint16_t input) {
    modVector = input >> 6;
  }

  void setSub (uint16_t input) {
    subLevel = input;
  }
  void setNoise (uint16_t input) {
    noiseLevel = input;
  }

  void setDetune (uint16_t input) {
    uint16_t temp = logPotentiometer(input);

    if (temp == 0) currentDetune = 0;
    else currentDetune = 1024 - temp;
  }
  void setOsc2Wave (uint16_t input) {
    osc2Wave = ((input >> 6) << 8);
  }
}



    