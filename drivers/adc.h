#pragma once

#include "../config.h"
#include "../functions.h"

#include "hardware/adc.h"
#include "hardware/dma.h"

#include "../random.h"


#define MUX_SEL_A       12
#define MUX_SEL_B       13
#define MUX_SEL_C       14
#define MUX_SEL_D       15

#define MUX_OUT_ADC     26

#define MAX_NOISE_ADDRESS   8

namespace ADC {
    namespace {
        float _core_temp;

        uint16_t _adc_value;
        uint16_t _adc_noise[MAX_NOISE_ADDRESS];
        uint8_t  _noise_address;
        uint8_t  _read_address = 2; // offset read address;
        uint16_t _values[MAX_KNOBS];

        uint8_t _mux_address;
        uint32_t _sample[MAX_KNOBS];
        uint32_t _output;


        void adc_temp_init(void) {
            adc_set_temp_sensor_enabled(true);
        }
        void adc_mux_init (void) {
            adc_gpio_init(MUX_OUT_ADC);
        }
        void adc_noise_init (void) {
            adc_gpio_init(MUX_OUT_ADC + 1); // Initiate unconnected pin
        }

        void adc_temp_select (void) {
            adc_select_input(4);
        }
        void adc_mux_select (void) {;
            adc_select_input(0);
        }
        void adc_noise_select (void) {
            adc_select_input(1);
        }
        void pin_init (uint8_t pin) {
            gpio_init(pin);
            // set the pins direction
            gpio_set_dir(pin, GPIO_OUT);

            // set the slew rate slow (for reducing amount of cross talk on address changes...)
            gpio_set_slew_rate(pin, GPIO_SLEW_RATE_SLOW);

            gpio_set_drive_strength(pin, GPIO_DRIVE_STRENGTH_2MA);
        }

        void read_onboard_temperature(void) {

            adc_temp_select();
            
            /* 12-bit conversion, assume max value == ADC_VREF == 3.3 V */
            const float conversionFactor = 3.3f / (1 << 12);

            float adc = (float)adc_read() * conversionFactor;
            _core_temp = 27.0f - (adc - 0.706f) / 0.001721f;

            // printf("Temp:       %.02fºC\n", _core_temp);

            // adc_set_temp_sensor_enabled(false);

            adc_mux_select();
        }


        // change these to just handle the sample filter with a reference, and then add new function for saving to the table.
        inline void NO_filter (uint16_t reading, uint8_t index) {
            _sample[index] = (reading >> 2);
        }
        inline void IIR_filter (uint16_t reading, uint8_t index) {
            _sample[index] = _sample[index] - (_sample[index]>>2) + reading;
        }
        inline void FIR_filter (uint16_t reading, uint8_t index) {
            // nothing to report yet...
            // just do no filter for now and fire an error
            NO_filter(reading, index);
            printf("There is no FIR filter implemented yet! Saving as RAW data. Go check the ADC...");
        }
        inline void increment_mux_address (void) {
            // sets the index to loop
            _mux_address = (_mux_address + 1) % MAX_KNOBS;
        }
        void read_mux (void) {
            // sets mux pins
            gpio_put(MUX_SEL_A, _mux_address & 1); 
            gpio_put(MUX_SEL_B, (_mux_address >> 1) & 1);
            gpio_put(MUX_SEL_C, (_mux_address >> 2) & 1);
            gpio_put(MUX_SEL_D, (_mux_address >> 3) & 1);

            // wait to read - allows settling time
            asm volatile("nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop \n nop");
            
            _adc_value = adc_read(); 
            

            // zeros mux for keys
            gpio_put(MUX_SEL_A, 0);
            gpio_put(MUX_SEL_B, 0);
            gpio_put(MUX_SEL_C, 0);
            gpio_put(MUX_SEL_D, 0);
        }
        void read_noise (void) {
            adc_noise_select();

            _adc_noise[_noise_address] = adc_read();
            _noise_address = (_noise_address + 1) % MAX_NOISE_ADDRESS;

            adc_mux_select();
        }
    }
    
    void init();
    void update();
    uint16_t value(int knob);
    float temp (void);
    uint8_t noise();
}
