#pragma once

#include "../config.h"

#include "../random.h"
#include "../queue.h"

#include "adsr.h"

#include "wavetable.h"
#include "resources.h"

#include "filter.h"


namespace SYNTH
{
  struct phase {

    private:

      volatile int32_t             increment = 0;
      volatile uint32_t            accumulator = 0;

    public: 
      void init(void) {
        increment = 0;
        accumulator = 0;
      }

      int32_t getIncrement(void) {
        return increment;
      }

      void setIncrement(int32_t input) {
        increment = input;
      }

      uint32_t getAccumulator(void) {
        return accumulator;
      }

      void setAccumulator(uint32_t input) {
        accumulator = input;
      }

      void updateAccumulator(int32_t input) {
        accumulator += input;
      }

  };

  struct OscillatorParameters
  {
    uint16_t waveOffset;

    uint16_t waveShape = 0;
    uint16_t waveVector = 0;

    uint16_t octave = 0;
    uint16_t pitchBend = 511;

    uint16_t level = 0xFFFF;
  };



  struct SharedParams
  {
    ADSR::Controls envelopeControls{SAMPLE_RATE};
    OscillatorParameters oscillator1;
    OscillatorParameters oscillator2;
    OscillatorParameters oscillatorN;

    int16_t detune = 0;

    int16_t modVibrato;
    uint16_t modTremelo;
    uint16_t modVector;

    uint16_t level  = 0xFFFF;

    void updateWaveshape (void) {
      oscillator1.waveOffset = oscillator1.waveShape + (oscillator1.waveVector + modVector);
      oscillator2.waveOffset = oscillator2.waveShape;
    }
  };

  namespace {
    volatile uint8_t voice_index;

    volatile bool recalculateIncrement = false;

    static SharedParams synthParameters;
  }


  struct Voice
  {

    ADSR::Envelope        ampEnvelope{synthParameters.envelopeControls.getAttack(), synthParameters.envelopeControls.getDecay(), synthParameters.envelopeControls.getSustain(), synthParameters.envelopeControls.getRelease()};

    uint8_t               index = 0;                // index of the voice in the synth

    uint8_t               note;                     // Midi Note number - used for filter voice
    bool                  gate = false;             // used for tracking a note that's released, but not finished.
    bool                  active = false;           // used for whole duration of note, from the very start of attack right up until the voise is finished
    bool                  refreshIncrement = false;

    // volatile uint64_t              caluclatedIncrement = 0;
    uint32_t              frequency = 0;            // Frequency in Hz << 10 (Q10)

    phase                 oscillator[2];

    bool                  hardSync = false;

    int32_t               sample = 0;

    void setIndex(uint8_t input) {
      index = input;
      oscillator[0].init();
      oscillator[1].init();
    }

    void updateIncrement(void) {
      
      if (!active) return;
      
      if (!refreshIncrement) return;

      volatile int64_t caluclatedIncrement = 0;

      // Changed the calculation to using a 64bit integer to allow for the higher values to be calculated without overflow
      caluclatedIncrement = (frequency * synthParameters.oscillator1.pitchBend) >> 12;
      caluclatedIncrement = caluclatedIncrement << synthParameters.oscillator1.octave;
      caluclatedIncrement = caluclatedIncrement << Q_SCALING_FACTOR;
      caluclatedIncrement = caluclatedIncrement / SAMPLE_RATE;
      oscillator[0].setIncrement(caluclatedIncrement);

      oscillator[1].setIncrement((oscillator[0].getIncrement() * synthParameters.detune) >> 9);

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

      oscillator[0].init();
      oscillator[1].init();

      QUEUE::releaseSend(index);
    }

    void checkHardSync(void) {
      if (!hardSync) return;

      if (oscillator[0].getAccumulator() % 256 == 0) oscillator[1].setAccumulator(0);
    }

    bool isActive(void) {
      return active;
    }
    
    bool isGate(void) {
      return gate;
    }
  

    int32_t process(void)
    {
      sample = 0;

      FILTER::update(index);
      ampEnvelope.update();

      if (active && ampEnvelope.isStopped()) {
        noteStopped();
      }

      oscillator[0].updateAccumulator(oscillator[0].getIncrement());             // update the phaseAccumulator
      oscillator[0].updateAccumulator(synthParameters.modVibrato); // add the vibrato

      sample = getWavetableInterpolated(oscillator[0].getAccumulator(), synthParameters.oscillator1.waveOffset);

      // checkHardSync(); // Not used, has an option to hard sync the oscillators, but has some issues with messing with tuning on main oscillator.

      oscillator[1].updateAccumulator(oscillator[1].getIncrement());
      oscillator[1].updateAccumulator(synthParameters.modVibrato);

      sample += (getWavetable(oscillator[1].getAccumulator(), synthParameters.oscillator2.waveOffset) * synthParameters.oscillator2.level) >> 16;


      sample += ((RANDOM::getSignal() >> 2) * synthParameters.oscillatorN.level) >> 16;

      sample /= 3;

      sample = (int32_t(sample) * int32_t(ampEnvelope.get())) >> 16;
      sample = (int32_t(sample) * int32_t((synthParameters.level - synthParameters.modTremelo))) >> 16;

      FILTER::process(index, sample);

      return sample;
    }
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
