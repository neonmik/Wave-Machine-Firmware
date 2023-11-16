#include "synth.h"

#include "modulation.h"
#include "filter.h"
#include "fx.h"

namespace SYNTH {

  uint16_t    currentWaveShape;
  uint16_t    lastWaveShape;

  uint16_t    currentWaveVector;
  uint16_t    lastWaveVector;

  uint16_t    currentPitchBend = 511;
  uint16_t    lastPitchBend;

  uint8_t     currentOctave = 0;
  uint16_t    lastOctave;

  uint16_t    currentDetune;
  uint16_t    lastDetune;

  uint32_t    currentAttack;
  uint32_t    currentDecay;
  uint32_t    currentSustain;
  uint32_t    currentRelease;
  // these are defaulted to outside the 10bit range so that when the EEPROM loads, it's always saved. (otherwise the currentAttack/currentDecay can be 0, which causes an overflow)
  uint16_t    lastAttack = 1024;
  uint16_t    lastDecay = 1024;
  uint16_t    lastSustain = 1024;
  uint16_t    lastRelease = 1024;

  int16_t     modVibrato;
  uint16_t    modTremelo;
  uint16_t    modVector;

  bool        subActive;
  bool        noiseActive;

  uint16_t    subLevel;
  uint16_t    noiseLevel;
  


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
    int32_t outputSample = 0;  // used to combine channel output
    
    MOD::update();

    // implemented this here so that it's set for the whole sample run...
    uint16_t waveOffset = (currentWaveShape + (currentWaveVector + modVector));
    uint16_t outputVolume = (volume - modTremelo);
    
    for(int c = 0; c < POLYPHONY; c++) {

      auto &channel = channels[c];
      // check if any waveforms are active for this channel
      if(channel.isActive()) {
        
        channel.calcIncrement();
        // increment the waveform position counter.
        channel.phaseAccumulator += channel.phaseIncrement;


        //this is where modVibrato is added... has to be here and not in the pitch scale as it would be lopsided due to logarithmic nature of freqencies.
        channel.phaseAccumulator += modVibrato;

        channel.ADSR.update();
        if (channel.isActive() && channel.ADSR.isStopped()) {
          channel.noteStopped();
          QUEUE::releaseSend(c);
          continue; // save processing this channel any futher
        }

        int32_t channelSample = 0;

        uint8_t oscillatorsActive = 0;
        
        channelSample += getWavetable(channel.phaseAccumulator, waveOffset); // >> Q_SCALING_FACTOR removed for interpolationg wavetable
        
        // Prototype oscillator modes:
        oscillatorsActive++;

        // Sub Mode
        if (subActive) {
          channelSample += (getWavetable((channel.phaseAccumulator >> 1), 0) * subLevel) >> 10; // Sub Sinewave oscillator test
          oscillatorsActive++;
        }

        // Noise Mode
        if (noiseActive){
          channelSample += ((RANDOM::getSignal() >> 2) * noiseLevel) >> 10;
          oscillatorsActive++;
        }

        // channelSample /= oscillatorsActive;
        channelSample /= 2;



        // apply ADSR
        channelSample = (int32_t(channelSample) * int32_t(channel.ADSR.get())) >> 16;

        // apply channel volume
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
    uint16_t temp = ((input >> 6) << 8); // the double bit shifts here are to quickly loose precision.
    if (temp == lastWaveShape) return;
    lastWaveShape = temp;
    currentWaveShape = temp;
  }
  void setWaveVector (uint16_t input) {
    currentWaveVector = input;
  }
  void setOctave (uint16_t input) {
    uint16_t temp = (input >> 8);
    if (lastOctave == temp) return;
    lastOctave = temp;
    currentOctave = temp;
  }
  void setPitchBend (uint16_t input) {
    if (lastPitchBend == input) return;
    lastPitchBend = input;
    currentPitchBend = logarithmicPitch(input);
  }

  void setAttack (uint16_t input) {
    if (lastAttack == input) return;
    lastAttack = input;
    currentAttack = calculateEndFrame(input << 2);
  }
  void setDecay (uint16_t input) {
    if (lastDecay == input) return;
    lastDecay = input;
    currentDecay = calculateEndFrame(input << 2);
  }
  void setSustain (uint16_t input) {
    if (lastSustain == input) return;
    lastSustain = input;
    currentSustain = (input << 6);
  }
  void setRelease (uint16_t input) {
    if (lastRelease == input) return;
    lastRelease = input;
    currentRelease = calculateEndFrame(input << 2);
  }
  uint32_t calculateEndFrame (uint32_t milliseconds) {
    return ((milliseconds + 1) * SAMPLE_RATE) / 1000; // + 1 so that it can never be 0, as it just creates noise.
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

  void toggleSub () {
    subActive = !subActive;
  }
  void toggleNoise () {
    noiseActive = !noiseActive;
  }

  void setSub (uint16_t input) {
    subLevel = input;
  }

  void setNoise (uint16_t input) {
    noiseLevel = input;
  }

}



    