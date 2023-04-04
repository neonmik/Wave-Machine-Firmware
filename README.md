# Beep-Machine-Firmware

Current nightly firmware for Beep Machine Hardware.

Things to implement:

- Multicore:
    - Need to create/reorganise layer between hardware and synth... reformat and streamline.

- Improve Settings funtionality:
    + Make everything that takes an input take it from a range of 0-1023
    - Refactor to allow saving, and also to improve code functionality - could be the key to Multicore
    \/ Make sure it always pulls values from presets (especially on start up) - this will require some tweaking of how the presets handle the input, and then make sure that it can pull that back correctly.
    - Make sure it only calls a para update when a values actually changed (probably need to improve the input value stabilities for this)


- Improve Oscillator script - current bugs include:
    + Add functions for all hardware controls
    - Add functions for all software controls (mod params)
    + Add and test functions for updating parameters, instead of accesing them directly from outside
    - Add logarithmic compression or soft clipping algorithm to the output sample (instead of hard cliping, but keep the option) to allow a better volume output/use more of the 12 bit output

- Improve Mod code:
    - fix control for preset/pagination
    - Move function calls to the synth side
    + Make outside variable updates go through functions
    + Make every function take 0-1023 for consistancy from the hardware layer

- Arp code:
    - fix control for preset/pagination - currently persists between presets no matter of the state of the new preset.

- Create a test script for hardware (ongoing with the use of DEBUG defines for printf, need to have a global debug level)

- Prove hardware functions:
    - MIDI
    - EEPROM
    - CV?

- Implement USB-MIDI and MIDI:
    - Notes
    - CC controls
    - Tempo



Future Implementaions and WIPs:

- Add double oscillators per voice (can be done currently, but can only be set inside of software and use one of the pre built waves (sine/square/triangle)).

- Lo-fi mode (Pots arent smoothed, allowing minute chanegs to alter pitch/other controls)

- Long button functions (Pages/Shift, LFO/?, Arp/?, Preset/Save) - implemented, but not chosen functions yet.

- Start-up settings (MIDI channel, other funtions?)

- Multicore support (probably hadware functions on one side, oscillators on another)

- Firmware upgrade procedure (hold reset button and connect to PC/Mac, drag and drop firmware) - Need to have a different name come up


Things already implemented:

+ Prove hardware functions (Pots, LEDs, Keys, Audio)
+ Test script for LEDs
    + Added function to show led test on startup if Preset button is held down
+ Intergrate/prove Oscillator code
    + ADSR not working for first oscillator/voice - added a minimum (10ms) limit on the AD settings... seemed to help. 
    + Sample peaking before output - down to the poor implementation of the default C signed/unsigned recasting. 
+ Improve Hardware files - current things to fix:
    + Create a better abstraction layer between the hardware and the software (synth) - currently theres issues passing hardware avriables to the software variables... ADSR/pitch. will also allow for better multicore support
+ Add Arp mode
+ Improve Arp functionality:
    + currently wont play only one note...
    + Needs direction functionality
    + currently has a random low note on release of arp (noticable in high octaves)
    + Improve ADSR - some confusion if you release key in attack stage, skips DS and jumps to release - this is standard behaviour for most synths.
+ Improve Pagination handling