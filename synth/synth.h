#pragma once

#include "../config.h"

#include "../random.h"
#include "../queue.h"

#include "adsr.h"

#include "wavetable.h"
#include "resources.h"


namespace SYNTH
{
  struct OscillatorParameters
  {
    uint16_t waveOffset;

    uint16_t waveShape = 0;
    uint16_t waveVector = 0;

    uint16_t octave = 0;
    uint16_t pitchBend = 511;

    uint16_t level = 0xFFFF;
  };

  // struct EnvelopeParameters
  // {
  //   uint16_t attack   = 0;
  //   uint16_t decay    = 0;
  //   uint16_t sustain  = 0;
  //   uint16_t release  = 0;
  // };

  struct SharedParams
  {
    ADSR::Controls envelopeControls{SAMPLE_RATE};
    OscillatorParameters oscillator1;
    OscillatorParameters oscillator2;
    OscillatorParameters oscillatorN;

    uint16_t detune = 0;

    int16_t modVibrato;
    uint16_t modTremelo;
    uint16_t modVector;

    uint16_t level  = 0xFFFF;

    void updateWaveshape (void) {
      oscillator1.waveOffset = oscillator1.waveShape + (oscillator1.waveVector + modVector);
      oscillator2.waveOffset = oscillator2.waveShape + (oscillator1.waveVector + modVector);
    }
  };

  namespace {
    volatile uint8_t voice_index;

    volatile bool recalculateIncrement = false;

    static SharedParams synthParameters;
  }


  struct Voice
  {

    ADSR::Envelope      ampEnvelope{synthParameters.envelopeControls.getAttack(), synthParameters.envelopeControls.getDecay(), synthParameters.envelopeControls.getSustain(), synthParameters.envelopeControls.getRelease()};

    uint8_t             index = 0;                // index of the voice in the synth

    uint8_t             note;                     // Midi Note number - used for filter voice
    bool                gate = false;             // used for tracking a note that's released, but not finished.
    bool                active = false;           // used for whole duration of note, from the very start of attack right up until the voise is finished
    bool                refreshIncrement = false;

    uint32_t            frequency = 0;            // Frequency in Hz << 8 (Q8)

    uint32_t            phaseIncrement = 0;
    uint32_t            phaseAccumulator = 0;

    int32_t             sample = 0;

    void setIndex(uint8_t input) {
      index = input;
    }

    void resetIncrement(void) {
      phaseIncrement = 0;
    }

    void updateIncrement(void) {
      
      if (!active) return;
      
      if (!refreshIncrement) return;

      phaseIncrement = ((((frequency * synthParameters.oscillator1.pitchBend) >> 10) << synthParameters.oscillator1.octave) << Q_SCALING_FACTOR) / SAMPLE_RATE;

      refreshIncrement = false;
    }

    void noteOn(uint8_t input_note) {
      gate = true; // Won't be needed if the Mod/Filter trigger is reworked
      active = true;
      note = input_note;
      frequency = getFrequency(note);
      refreshIncrement = true;

      ampEnvelope.triggerAttack();
    }
    
    void noteOff(void) {
      gate = false; // Won't be needed if the Mod/Filter trigger is reworked

      ampEnvelope.triggerRelease();
    }
    
    void noteStopped(void) {
      active = false;

      note = 0;
      frequency = 0;

      resetIncrement();
      phaseAccumulator = 0;

      QUEUE::releaseSend(index);
    }

    bool isActive(void) {
      return active;
    }
    
    bool isGate(void) {
      return gate;
    }
  
    // look at implenting an index variable and a counter above this to reduce calls to calculate increment. this could reduced the 
    int32_t process(uint8_t voiceIndex)
    {
      if (!active) return 0;

      phaseAccumulator += phaseIncrement;             // update the phaseAccumulator
      phaseAccumulator += synthParameters.modVibrato; // add the vibrato

      ampEnvelope.update();

      if (active && ampEnvelope.isStopped()) {
        noteStopped();
        return 0;
      }

      sample = getWavetableInterpolated(phaseAccumulator, synthParameters.oscillator1.waveOffset);

      uint32_t phase;
      
      if (synthParameters.detune == 0) {
        phase = (phaseAccumulator >> 1);
      } else {
        phase = phaseAccumulator + (phaseAccumulator / synthParameters.detune);
      }
      sample += (getWavetable(phase, synthParameters.oscillator2.waveOffset) * synthParameters.oscillator2.level) >> 16;


      sample += ((RANDOM::getSignal() >> 2) * synthParameters.oscillatorN.level) >> 16;

      sample /= 3;

      sample = (int32_t(sample) * int32_t(ampEnvelope.get())) >> 16;
      sample = (int32_t(sample) * int32_t((synthParameters.level - synthParameters.modTremelo))) >> 16;

      return sample;
    }

    // Voice(SharedParams &synthParameters) : synthParameters(synthParameters) {}
    Voice() {}
  };

  class Synthesizer
  {
  public:
    Voice voices[POLYPHONY];


    int32_t sample = 0;

    Synthesizer()
    {
      // Tell each voice which slot it is. This is used for releasing the voice when it's finished.
      for (int i = 0; i < POLYPHONY; i++)
      {
        voices[i].setIndex(i);
      }
    }
    ~Synthesizer() {}
  };

  static Synthesizer synth;

  void voiceOn(uint8_t voice, uint8_t note);
  void voiceOff(uint8_t voice);

  void init(void);
  uint16_t process(void);
  void calculateIncrements(void);

  void setWaveShape(uint16_t input);
  void setWaveVector(uint16_t input);
  void setOctave(uint16_t input);
  void setPitchBend(uint16_t input);

  void setAttack(uint16_t input);
  void setDecay(uint16_t input);
  void setSustain(uint16_t input);
  void setRelease(uint16_t input);

  void modulateVibrato(uint16_t input);
  void modulateTremelo(uint16_t input);
  void modulateVector(uint16_t input);

  void setSub(uint16_t input);
  void setNoise(uint16_t input);

  void setDetune(uint16_t input);
  void setOsc2Wave(uint16_t input);
}
