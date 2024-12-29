#pragma once

class Phase {
    private:

      int32_t     increment;
      uint32_t    accumulator;

    public: 

      Phase() : increment(0), accumulator(0) { }

      void reset(void) {
        increment = 0;
        accumulator = 0;
      }

      int32_t getIncrement(void) const {
        return increment;
      }

      void setIncrement(int32_t input) {
        increment = input;
      }

      void resetIncrement(void) {
        increment = 0;
      }

      uint32_t getAccumulator(void) const {
        return accumulator;
      }

      void setAccumulator(uint32_t input) {
        accumulator = input;
      }

      void resetAccumulator(void) {
        accumulator = 0;
      }

      void updateAccumulator(int32_t input) {
        accumulator += input;
      }

  };