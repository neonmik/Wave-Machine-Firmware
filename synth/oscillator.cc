#include "oscillator.h"

uint32_t prng_xorshift_state = 0x32B71700;

uint32_t prng_xorshift_next() {
    uint32_t x = prng_xorshift_state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    prng_xorshift_state = x;
    return x;
}

int32_t prng_normal() {
    // rough approximation of a normal distribution
    uint32_t r0 = prng_xorshift_next();
    uint32_t r1 = prng_xorshift_next();
    uint32_t n = ((r0 & 0xffff) + (r1 & 0xffff) + (r0 >> 16) + (r1 >> 16)) / 2;
    return n - 0xffff;
}

void Oscillator::set (uint16_t frequency, uint8_t octave) {
    _frequency = frequency;
    _octave = octave;
}
void Oscillator::clear () {
    _frequency = 0;
    _octave = 0;

}

int32_t Oscillator::process () {
    // increment the waveform position counter. this provides an
    // Q16 fixed point value representing how far through
    // the current waveform we are
    _waveform_offset += (((((_frequency * _pitch_bend)>>9) << _octave) * 256) << 8) / _sample_rate;

    //this is where vibrato is added... has to be here and not in the pitch scale as it would be lopsided due to logarithmic nature of freqencies.
    _waveform_offset += _vibrato;

    if(_waveform_offset & 0x10000) {
      // if the waveform offset overflows then generate a new
      // random noise sample
      _noise = prng_normal();
    }

    _waveform_offset &= 0xffff;

    uint8_t waveform_count = 0;
    int32_t sample = 0;


    if(_oscillator & OscillatorMode::NOISE) {
        sample += _noise;
        waveform_count++;
    }

    if(_oscillator & OscillatorMode::SAW) {
        sample += (int32_t)_waveform_offset - 0x7fff;
        waveform_count++;
    }

    // creates a triangle wave of ^
    if (_oscillator & OscillatorMode::TRIANGLE) {
        if (_waveform_offset < 0x7fff) { // initial quarter up slope
            sample += int32_t(_waveform_offset * 2) - int32_t(0x7fff);
        } else { // final quarter up slope
            sample += int32_t(0x7fff) - ((int32_t(_waveform_offset) - int32_t(0x7fff)) * 2);
        }
        waveform_count++;
    }

    if (_oscillator & OscillatorMode::SQUARE) {
        sample += (_waveform_offset < _pulse_width) ? 0x7fff : -0x7fff;
        waveform_count++;
    }

    if(_oscillator & OscillatorMode::SINE) {
        // the sine_waveform sample contains 256 samples in
        // total so we'll just use the most significant bits
        // of the current waveform position to index into it
        sample += sine_waveform[(_waveform_offset >> 8)];
        waveform_count++;

    }

    if(_oscillator & OscillatorMode::WAVETABLE) {

        // the wavetable sample contains 256 samples in
        // total so we'll just use the most significant bits
        // of the current waveform position to index into it
        sample += wavetable[(_waveform_offset >> 8) + _wavetable_index];
        waveform_count++;

        // OLD Arbitary Waveform? Not sure how it was meant to work, but it didnt...
        // sample += channel.wave_buffer[channel.wave_buf_pos];
        // if (++channel.wave_buf_pos == 64) {
        //   channel.wave_buf_pos = 0;
        //   if(channel.wave_buffer_callback)
        //       channel.wave_buffer_callback(channel);
        // }
        // waveform_count++;
    }

    // divide the sample by the amount of waveforms - good for multi oscillator voices
    sample = sample / waveform_count;

    // apply ADSR
    // sample = (int32_t(sample) * int32_t((channel.ADSR.get_adsr()) >> 8)) >> 16;

    // // apply channel volume
    // sample = (int32_t(sample) * int32_t(channel.volume)) >> 16;

    // apply channel filter
    // if (filter_enable) {
    //   sample += (sample - channel.filter_last_sample) * filter_epow;
    // }
    // channel.filter_last_sample = sample;

    return sample;
}