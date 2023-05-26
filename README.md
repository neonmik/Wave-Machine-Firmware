# Beep-Machine-Firmware

Current nightly firmware for Beep Machine Hardware.


Updates and Bugfixes:

- Multicore
    - Move Note_priority back to core1 - Take the time pressiure off core1, and has it send voice assignments via a queue. 

- Improve Settings funtionality:
    - Develop way of exporting presets (probably needs to be linked in to either MIDI or, better yet, some kind of USB mounted storage)
    - LOW PRIORITY - Make sure it only calls a para update when a values actually changed (_probably_ 100% need to improve the input value stabilities for this)
    
- Improve Oscillator script - current bugs include:
    - Improve tuning - currently theres a drift in tuning, more than likely down to using interger instead of floating point numbers for the MIDI2Freq calculations, most apparent on Preset 5 due to the fact that it's pitched down with the octave setting and pitch shift.
    - Finesse soft start code - currently takes too long to get going and still isnt perfect.
    - Add logarithmic compression or soft clipping algorithm to the output sample (instead of hard cliping, but keep the option) to allow a better volume output/use more of the 12 bit output
  
- Improve USB MIDI/ Implement MIDI hardware:
    - Add midi_task() alongside usb_midi_task(). 
    - Write MIDI hardware code.

    - Tidy up MIDI processing code, could be more efficient.
    - Map more CC values and check they're working. 

    - Create BEAT_CLOCK sync:- pretty sure Clock is getting in, just need to implement the actual clock syncing. 

    - Add MIDI channel assignment to start up
    - Add a MIDI settings structure and a way of saving persistant data (via EEPROM)

    - Fix MIDI in note calls (currently need a key press first to allow midi) - implement a way to interface note calls directly to the MAILBOX. (maybe a kind of shared _changed flag inside the mailbox, set able from both MIDI and KEYS for now)
    - Fix MIDI in CC calls (currently get stuck if page is open on hardware controls) - find a way to interface MIDI messages and hardware messages and find a way to assign different locks.

    

- Create a test script for hardware (ongoing with the use of DEBUG defines for printf, need to have a global debug level)

- Prove hardware functions:
    - MIDI
    - CV?





Future Implementaions and WIPs:

- Add hardware controls for:
    - Factory Reset
    - Setting BPM

- Improve ADSR code:
   - Make the code more portable:- currently the calculations for ADSR times are done in the synth module, but would be good to move them into the ADSR module. I need global controls, but to be able to trigger phases of each voice seperately.
 
- Improve Mod code:
    - Add a ramp down feature when switching between destinations - could be difficult. 
    - Add a temp sync function.
    - _currently_ impossible due to over extending the processor... Add ADSR... this could be implemented by initalising an ADSR class in the mod code applying to the final mod output, then include that in Note_Priority. This can be MOD::Attack() in the note on section and MOD::Release() in the note off, controlled by an "if (notes_active)" statment and a counter for how many voice are currently active.

- Arp code:
    - Add proper Latch feature that can work on a time based chord played type thing - I.e. you chould play two notes and then a few mills later play 5 notes and the original two notes would clear and it would hold the 5 new notes, and so on. might need some sort of time out feature.
    - Keep Hold function (for sustain pedal CC64) but make sure it can clear any notes that arent playing when released
    - With Hold/Latch engaged (only):- If you play a 2 octave C7, followed by a 2 oct Dm7, fine, but if you then play another 2 octave C7, the note organised gets confused. Something to do with the return on double notes I believe... mayeb move the reorganizing to the end of the Note Priority update loop.
    
- Add Portomento Mode

- Lo-fi mode (Pots arent smoothed, allowing minute chanegs to alter pitch/other controls)
    - could use the prng function from MOD as using the dither function that uses this on the trem output added noise, that noise could also be used to make the pitch unstable at a desired amount?

- Long button functions (Pages/Shift, LFO/?, Arp/?, Preset/Save) - implemented, but not chosen functions yet.

- Start-up settings (MIDI channel, other funtions?)

- Firmware upgrade procedure (hold reset button and connect to PC/Mac, drag and drop firmware) - Need to have a different name come up


- Add a paraphonic filter - _is_ could be too much for MCU at the minute, but could be done similar to mod with adjustable ADSR, just need to reconfigure UI layout really...

- Add double oscillators per voice (can be done currently, but can only be set inside of software and use one of the pre built waves (sine/square/triangle)).



Things already implemented:

+ Proven hardware functions (Pots, LEDs, Keys, Audio)
    + EEPROM
        + Improved Save handling, moved code preset saving code into here to make the UI more friendly:- Rewrite lower storgae code to use PRESET struct instead of breaking it down into bytes (was for an issue due to page size I believe)
    + LEDs
        + Test script for LEDs
            + Added function to show led test on startup if Preset button is held down
    + Buttons
    + Keys

+ Oscillator bug fixes:
    + Added functions for all software controls (mod params)
    + Added a softstart to the Oscillator code - stops it popping when turned on
    + Add functions for all hardware controls
    + Add and test functions for updating parameters, instead of accesing them directly from outside
    + Sample peaking before output - down to the poor implementation of the default C signed/unsigned recasting. 


+ Hardware files bug fixes:
    + Create a better abstraction layer between the hardware and the software (synth) - currently theres issues passing hardware avriables to the software variables... ADSR/pitch. will also allow for better multicore support

+ Arp functionality bug fixes:
    + Bugfix: fixed bug in Arp noter removal logic that cause strange behaviour and a comile warning.
    + Arp can't keep up if at high speeds (above 1/16, or 1/32)... ONLY while on arp page:- MUST but the update of the controls is causing an issue, need to add multicore mailbox/greater issue of unstable controls... 
    + When the range is set to anything above 0 only the first octave of the arp has proper release - think it's to do with the note_clear function in the oscillator (definitely was)
    + fix control for preset/pagination - currently persists between presets no matter of the state of the new preset.
    + currently wont play only one note...
    + Needs direction functionality
    + currently has a random low note on release of arp (noticable in high octaves)
    + Add Arp mode

+ Improve Settings functionality
    + Factory Restore function working, just need to implement in controls
    + Load Current Factory Presets to Factory Space
    + Should add a "Factory Reset" state in UI (using Mutable Instruments style system state) to allow reset of all sounds to default (currently 8 presets of standard sinewave synth, but eventually 8 cool sound presets)
    + Develop 8 cool presets.
    + FIX - when jumping about in pages, if set to the heighest value (1023) sometimes it doen't latch when you go back to the page :- was down to storing last value from other pages, basically couldnt call cause it was the same, even though it was a fresh page.
    + Make everything that takes an input take it from a range of 0-1023
    + Refactor to allow saving, and also to improve code functionality
    + Improve Pagination handling
    _- Make sure it always pulls values from presets (especially on start up) - this will require some tweaking of how the presets handle the input, and then make sure that it can pull that back correctly.

+ ADSR bug fixes:
    + Fixed an overflow issue with multiple notes being in an extended Decay phase causing the output sample to be limitied cause audio artifacts:- Bug: Clipping output signal - When running ARP full speed/range/DOWN-UP with 8 notes, ADSR attack min, release max, and then start turning decay up, it hard clips... think this is down to attack getting to full value (0xffff) instead of (0x6fff/0x7fff), so when all voices push that mid 16bit in value, the whole output sample volume breaks 16 bit, and therefore clips before getting downsampled.)
     + Removed any useless calls to synth voice stuff so can be used more universally (currently planning on adding to Mod) - currently get passed values for notes and stuff, kinda unnecessary for actual ADSR, but used in this implementaion to clear the voice, could just be donw by checking but I thought it was stopping code. could just use "if (ADSR::isStopped()) Voice::note_clear" in the saudio process code.
     + ADSR not working for first oscillator/voice - added a minimum (10ms) limit on the AD settings... seemed to help. 
     + Improved ADSR - some confusion if you release key in attack stage, skips DS and jumps to release - this is standard behaviour for most synths by testing some.

+ Mod bug fixes:
    + Added a linear to exponential curve for the Rate function. Can now rate between 0.1Hz and 5000Hz with the lower end of the range being finest, and the higher end coarsest.
    + This was due to a fault wavetable for the Mod code (had an overflow in the sine wave):- Mod overflowing vector/wavetable index I think - if the mod is set slow/max depth/vector output and the actual vector control or wavetable is higher up, it overflows the table and freaks out. Constrain the wavetable indexing.
    + This was due to the output overflowing when the depth was applied, effectivaly doubling the output frequency, but unevenly:- Oscilaltor folds down at the top of range (can be seen at 0.1Hz on vibrato with a tuner - when pressing C with depth to full, it F# to F, but does a little duck away from F at the "top")
    + Tidy up code to remove unnecessary stuff.
        + Move PRNG to its own files, keeps it tidy and also can then be used by synth side
        + Removed int8_output/uint10_output... not really needed anymore.
    + Fixed - was down to the placement of the depth calculation. Only happens when switching between Outputs now... Vibrato doesn't settle on 0 properly causing tuning issues when switching outputs and LFO on/off
    + Wave/Shape control currently doesnt work
    + Improved algorithm:-
        + Mad inputs tidier/more unified - created a struct to hold all of the config for each destination
        + this isn't going to work, it's easier to make each input accept signed 16bit number (direct from the oscillator) _Make a switchable output between signed and unsign methods (Vib == Signed, Trem||Vector == Unsigned)_
    + Mod is now opperated by the Synth code -> Move update closer to synth/dac code:- could probably do with being intergrated like ADSR 
    + Fix control for preset/pagination
    + Make outside variable updates go through functions
    + Make every function take 0-1023 for consistancy from the hardware layer

+ Multicore:
    + Setup Note_Priority tracking system so that it can be moved back to core1
    + Setup referenced function calls for Synth/Mod/Arp controls (seemed to work way better than using the mailbox system and then calling functions on the other side)
    + Added Mailbox for note handling
    + Created/reorganise layer between hardware and synth.
    + Finally added Multicore support (hadware functions on one side, synth/dac on another)

+ USB MIDI/MIDI:
    + MIDI Pitchbend is working.
    + MIDI CC inputs and values are working:
        + Mod Wheel (CC2)
        + Volume (CC7)
        + Wavetable (CC70)
        + Vector (CC71)
        + Release (CC72)
        + Attack (CC73)
        + Decay (CC75)
    + Added basic functions for testing (Note On, Note Off and Clock) all proven. 
    + USB-MIDI is now functional! 