# Beep-Machine-Firmware

Current nightly firmware for Beep Machine Hardware.

Things to implement:

- Prove hardware functions (MIDI)
- Improve Oscillator script - current bugs include:
    - ADSR not working for first oscillator/voice
    - Sample peaking before output (fine with one voice, but after a few its just a mess)
- Create a test script for hardware (ongoing with the use of DEBUG defines for printf, need to have a global debug level)
- Implement USB-MIDI and MIDI


Future Implementaions:

- Add double oscillators per voice
- Tidy codebase to allow for cleaner abstraction, and therfore allowing multicore support
- Firmware upgrade procedure (hold reset button and connect to PC/Mac, drag and drop firmware) - Need to have a different name come up
- Lo-fi mode (Pots arent smoothed, allowing minute chanegs to alter pitch/other controls)
- Long button functions (Pages/Shift, LFO/?, Arp/?, Preset/Save)
- Start-up settings (MIDI channel, other funtions?)
- Multicore support (probably hadware functions on one side, oscillators on another)

Things already implemented:

- Prove hardware functions (Pots, LEDs, Keys, Audio)
- Test script for LEDs
- Intergrate/prove Oscillator code