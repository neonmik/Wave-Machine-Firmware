#include "adc.h"

namespace ADC {
    void init() {

        adc_init();

        adc_gpio_init(ADC_MUX_PIN);
        adc_gpio_init(ADC_NOISE_PIN);
        adc_gpio_init(ADC_VOLTAGE_PIN);

        // Before setting up the pots, do a quick read of the startup temp
        readTemperature();

        // Read multiple times to fill the buffers
        readNoise();
        readNoise();
        readNoise();
        readNoise();
        readNoise();
        readNoise();
        readNoise();
        readNoise();
        readNoise();

        // read the battery voltage multiple times, if read once at this point it will be incorrect
        readBattery();
        readBattery();
        readBattery();
        readBattery();

        updateBattery();
    }

    void update() {
        // Get the address from MUX and mask it to keep only the lowest two bits
        uint8_t address = (MUX::getAddress() & 3);

        uint16_t adcValue = readMux();
        
        applyHysteresis(rawSamples[address], adcValue, HYSTERESIS_WEIGHTING);

        iirFilter(rawSamples[address], address);

        // moves filtered sample to the adc array
        outputSamples[address] = map_constrained(filteredSamples[address] >> IIR_FILTER_WEIGHTING, INPUT_RANGE_MIN, INPUT_RANGE_MAX, KNOB_MIN, KNOB_MAX);

        adcNoise[noiseWriteAddress] = adcValue & 0x3;
        noiseWriteAddress = (noiseWriteAddress + 1) % MAX_NOISE_READINGS;

        readNoise();
    }
    
    uint16_t value(int knob) {
        return outputSamples[knob]; // downshifting for better system wide performance
    }
    uint8_t noise() {
        noiseReadAddress = (noiseReadAddress + 1) % MAX_NOISE_READINGS;
        return adcNoise[noiseReadAddress];
    }
    float temperature() {
        return coreTemperature;
    }
    float battery() {
        return batteryVoltage;
    }
    void updateBattery() {
        
        readBattery();

        if (ADC::battery() < MINIMUM_BATTERY_VOLTAGE) {
            // battery is low, do something
            printf("Battery is low: %f\n", ADC::battery());
            printf("Please change or connect to USB.\n");
            batteryLow = true;
        } else { 
            batteryLow = false;
        }
    }
    bool isBatteryLow() {
        return batteryLow;
    }
    void printADCValues() {
        printf("ADC Values:");
        for (int i = 0; i < MAX_KNOBS; i++) {
            printf("  %d  |", outputSamples[i]);
        }
        printf("\n");
    }
}