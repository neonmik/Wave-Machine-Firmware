# Beep-Machine-Firmware

Current nightly firmware for Beep Machine Hardware.

Things to implement:

- Multicore:
    - Need to create/reorganise layer between hardware and synth... reformat and streamline.

- Improve Settings funtionality:
    - Make sure it only calls a para update when a values actually changed (probably need to improve the input value stabilities for this)
    


- Improve Oscillator script - current bugs include:
    - Finesse soft start code - currently takes too long to get going.
    - Add functions for all software controls (mod params)
    - Add logarithmic compression or soft clipping algorithm to the output sample (instead of hard cliping, but keep the option) to allow a better volume output/use more of the 12 bit output

- Improve Mod code:
    - fix control for preset/pagination
    - Move function calls to the synth side
    - Move update closer to synth/dac code

- Arp code:
    - Arp can't keep up if at high speeds (above 1/16, or 1/32)... ONLY while on arp page:- MUST but the update of the controls is causing an issue, need to add multicore mailbox/greater issue of unstable controls... 
    - If you play a 2 octave C7, followed by a 2 oct Dm7, fine, but if you then play another 2 octave C7, the note organised gets confused. Something to do with the return on double notes I believe... mayeb move the reorganizing to the end of the Note Priority update loop.
    

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

+ Proven hardware functions (Pots, LEDs, Keys, Audio)
    + LEDs
        + Test script for LEDs
            + Added function to show led test on startup if Preset button is held down
    + Buttons
    + Keys

+ Oscillator bug fixes:
    + Added a softstart to the Oscillator code - stops it popping when turned on
    + Add functions for all hardware controls
    + Add and test functions for updating parameters, instead of accesing them directly from outside
    + ADSR not working for first oscillator/voice - added a minimum (10ms) limit on the AD settings... seemed to help. 
    + Sample peaking before output - down to the poor implementation of the default C signed/unsigned recasting. 


+ Hardware files bug fixes:
    + Create a better abstraction layer between the hardware and the software (synth) - currently theres issues passing hardware avriables to the software variables... ADSR/pitch. will also allow for better multicore support

+ Add Arp mode
+ Arp functionality bug fixes:
    + When the range is set to anything above 0 only the first octave of the arp has proper release - think it's to do with the note_clear function in the oscillator (definitely was)
    + fix control for preset/pagination - currently persists between presets no matter of the state of the new preset.
    + currently wont play only one note...
    + Needs direction functionality
    + currently has a random low note on release of arp (noticable in high octaves)
    + Improve ADSR - some confusion if you release key in attack stage, skips DS and jumps to release - this is standard behaviour for most synths.

+ Improve Pagination handling
+ Improve Settings functionality
    + FIX - when jumping about in pages, if set to the heighest value (1023) sometimes it doen't latch when you go back to the page :- was down to storeing last value from other pages, basically couldnt call cause it was the same, even though it was a fresh page.
    + Make everything that takes an input take it from a range of 0-1023
    + Refactor to allow saving, and also to improve code functionality
    _- Make sure it always pulls values from presets (especially on start up) - this will require some tweaking of how the presets handle the input, and then make sure that it can pull that back correctly.

+ Mod bug fixes:
    + Make outside variable updates go through functions
    + Make every function take 0-1023 for consistancy from the hardware layer