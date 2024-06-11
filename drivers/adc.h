#pragma once

#include "../config.h"
#include "../functions.h"

#include "mux.h"

#include "hardware/adc.h"
// #include "hardware/dma.h"

#include "../random.h"


namespace ADC {

    enum ADC_GPIO : uint8_t {
        ADC_MUX_PIN         = 26,
        ADC_NOISE_PIN       = 27,
        ADC_VOLTAGE_PIN     = 29,
    };

    enum ADC_CHANNEL : uint8_t {
        ADC_MUX_CHANNEL         = 0,
        ADC_NOISE_CHANNEL       = 1,
        ADC_VOLTAGE_CHANNEL     = 3,
        ADC_TEMPERATURE_CHANNEL = 4
    };

    constexpr   uint8_t     MAX_NOISE_READINGS = 32;

    constexpr   uint8_t     HYSTERESIS_WEIGHTING = 1;
    constexpr   uint8_t     IIR_FILTER_WEIGHTING = 6;

    // these are set inside the range of the ADC to compensate for the noise floor (on the minimum) and battery power (on the maximum)
    constexpr   uint16_t    INPUT_RANGE_MIN = 10;
    constexpr   uint16_t    INPUT_RANGE_MAX = 4090;

    constexpr   float       MINIMUM_BATTERY_VOLTAGE = 3.58;

    namespace {
        float       coreTemperature;
        float       batteryVoltage;

        const float conversionFactor = 3.27f / (1 << 12);

        uint16_t adcNoise[MAX_NOISE_READINGS];
        uint8_t  noiseWriteAddress;
        uint8_t  noiseReadAddress = 2; // offset read address;

        int rawSamples[MAX_KNOBS];
        int filteredSamples[MAX_KNOBS];
        int outputSamples[MAX_KNOBS];

        bool batteryLow = false;

        

        void channelSelect (uint8_t channel) {
            adc_select_input(channel);
        }

        void readTemperature(void) {
            
            adc_set_temp_sensor_enabled(true);

            channelSelect(ADC_CHANNEL::ADC_TEMPERATURE_CHANNEL);
            
            uint16_t reading = adc_read();

            float adc = (float)reading * conversionFactor;

            coreTemperature = 27.0f - (adc - 0.706f) / 0.001721f;

            adc_set_temp_sensor_enabled(false);

            channelSelect(ADC_CHANNEL::ADC_MUX_CHANNEL);
        }

        void readBattery(void) {
            channelSelect(ADC_CHANNEL::ADC_VOLTAGE_CHANNEL);

            uint16_t reading = adc_read();
            sleep_ms(2); // wait for the ADC to settle
            reading = adc_read(); // repeat to ensure correct

            float adc = (float)reading * conversionFactor;

            batteryVoltage = (adc * 3.0f);

            channelSelect(ADC_CHANNEL::ADC_MUX_CHANNEL);
        }

        inline void iirFilter (uint16_t reading, uint8_t index) {
            filteredSamples[index] = filteredSamples[index] - (filteredSamples[index] >> IIR_FILTER_WEIGHTING) + reading;
        }
        
        uint16_t readMux (void) {
            return adc_read(); 
        }
        void readNoise (void) {
            channelSelect(ADC_CHANNEL::ADC_NOISE_CHANNEL);

            adcNoise[noiseWriteAddress] = adc_read();
            noiseWriteAddress = (noiseWriteAddress + 1) % MAX_NOISE_READINGS;

            channelSelect(ADC_CHANNEL::ADC_MUX_CHANNEL);
        }
    }
    
    void init();
    void update();
    uint16_t value(int knob);
    
    float temperature (void);
    
    uint8_t noise();

    float battery(void);
    void updateBattery(void);
    bool isBatteryLow (void);
}
