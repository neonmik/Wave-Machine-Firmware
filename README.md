# Beep-Machine-Firmware

Current nightly firmware for Beep Machine Hardware.

Things to implement:

- Prove hardware functions (MIDI)
- Improve Hardware files - current things to fix:
    - Create a better abstraction layer between the hardware and the software (synth) - currently theres issues passing hardware avriables to the software variables... ADSR/pitch. will also allow for better multicore support
    
- Improve Oscillator script - current bugs include:
    - Add logarithmic compression or soft clipping algorithm to the output sample (instead of hard cliping, but keep the option) to allow a better volume output/use more of the 12 bit output
- Create a test script for hardware (ongoing with the use of DEBUG defines for printf, need to have a global debug level)
- Implement USB-MIDI and MIDI


Future Implementaions:

- Add double oscillators per voice
- Tidy codebase to allow for cleaner abstraction, and therfore allowing multicore support
- Firmware upgrade procedure (hold reset button and connect to PC/Mac, drag and drop firmware) - Need to have a different name come up
- Lo-fi mode (Pots arent smoothed, allowing minute chanegs to alter pitch/other controls)
- Long button functions (Pages/Shift, LFO/?, Arp/?, Preset/Save) - implemented, but not chosen functions yet.
- Start-up settings (MIDI channel, other funtions?)
- Multicore support (probably hadware functions on one side, oscillators on another)

Things already implemented:

- Prove hardware functions (Pots, LEDs, Keys, Audio)
- Test script for LEDs
- Intergrate/prove Oscillator code
    - ADSR not working for first oscillator/voice - possibly to do with my implementaion of the 
    - Sample peaking before output - down to the poor implementation of the default C signed/unsigned recasting