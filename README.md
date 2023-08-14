# Wave-Machine-Firmware

Current nightly firmware for Wave Machine Hardware.


- Alpha Release bugfixes:

    - Bug: Filter still releases if you're holding a chord, say of thre notes, and then play a few notes above... assuming down to the simple algorithm not performing correctly when full.
    - Bug: Notes and releases actint strangly accross preset changes - some notes can get stuck.
    - Bug: MIDI timecode sync not working properly - investigate. Seems to be in the beat clock? looks like Midi clock is coming through right... 


- Updates and Bugfixes:

    - Feature: Figure sustain pedal algorithm out. Needs to be before Arp or Priority allocation but also be able to keep up with voice allocation? probably needs to control two seperate algorithms in seperate sections, like I've started.

    - Arp & Filter/Mod Envelope should have an option to be from the start of any pressed notes, so that the envelope can ope slowly up on a playing Arp.

    - Improve Controls funtionality:
        - Change the layout of controls:
            - 1: OSC, 2: LFO, 3: FLT, ALL(4): ARP(For Now), shift for all should be ENV control, and Active should control on off of all associated functions.
        - Holding Preset should save
        - Shift should be holding Page?
        - 
        - Develop way of exporting Presets (probably needs to be linked in to either MIDI or, better yet, some kind of USB mounted storage)
        
    - Improve Oscillator script - current bugs include:
        - Finesse soft start code - currently takes too long to get going and still isnt perfect.
    
    - Improve USB MIDI/ Implement MIDI hardware:

        - Test UART-MIDI
        - Test UART

        - Tidy up MIDI processing code, could be more efficient.

        - Map more CC values and check they're working. 


        - Add MIDI channel assignment to start up

        - Add a MIDI settings structure and a way of saving persistant data (via EEPROM)



        - Fix MIDI in CC calls (currently get stuck if page is open on hardware controls) - find a way to interface MIDI messages and hardware messages and find a way to assign different locks.
        
        - Add dynamic setting of MIDI timeout. Currently set to longest possible time out (670000µs for minimum pulse at 20BPM)... just need some kind of calculation so that you get a rough average of say like 8 or 10 pusles + 1000?
        
        - Bugfix: quantize arp? so it always starts right, especially between changing divisions

        - Add MIDI Clock out. Needs to be 24ppqn, and have high priority timing wise, and obvs be in sync with the synth... (last bit might be more tricky).

        


    - Prove hardware functions:
        - MIDI
        - CV?





- Future Implementaions and WIPs:

    - Add hardware controls for:
        - Factory Reset
        - Setting BPM

    - Improve ADSR code:
        - Make the code more portable:- currently the calculations for ADSR times are done in the synth module, but would be good to move them into the ADSR module. I need global controls, but to be able to trigger phases of each voice seperately.
    
    - Improve Mod code:
        - Try making it Poly - I think the sample rate can be reduced by 8 (6kHz) and that should allow every voice to have its own poly Mod. Would probably need a reset for when notes are released, so that you can really hear the difference.

        - Add a tempo sync function.
        - Add ADSR... this could be implemented by initalising an ADSR class in the mod code applying to the final mod output, then include that in Note_Priority. This can be MOD::Attack() in the note on section and MOD::Release() in the note off, controlled by an "if (notes_active)" statment and a counter for how many voice are currently active.
        - Add a ramp down feature when switching between destinations - could be difficult. 
    
    - Improve Filter code: 
        - Improve modulation inputs for controls.
        - Add a switch for direction of envelope (like the Modulation setup with different types of filter selecting differing paths for the output)

    - Arp code:
        - Add a setting for note length within the Arp -    currently plays note for a beat, then a beat rest, then another and so on.
                                                            I find this more musical, but I believe most synths dont have this?
        - Add a setting for patterns - so that its not just straight Quarter/Sixteenth notes etc. Think 90's/00's timberland synths
        - Add a swing setting.
        - Re-add chord arp - will work great with patterns too.
        - Add proper Latch feature that can work on a time based chord played type thing - I.e. you chould play two notes and then a few mills later play 5 notes and the original two notes would clear and it would hold the 5 new notes, and so on. might need some sort of time out feature.
        - Keep Hold function (for sustain pedal CC64) but make sure it can clear any notes that arent playing when released
        - With Hold/Latch engaged (only):- If you play a 2 octave C7, followed by a 2 oct Dm7, fine, but if you then play another 2 octave C7, the note organised gets confused. Something to do with the return on double notes I believe... mayeb move the reorganizing to the end of the Note Priority update loop.

    - Add Portomento Mode: Should be added form the note-handling script (adding a portomento flag via the priority script, and having a new note, but dont clear old note freq in the message?) then a portamento time control, which then slides the note freq from old to new. 
        - check out yoshimi github
        if (porto) {
            if (!aligned) {
                if (oldfreq > newfreq) oldfreq -= (portamento_inc/2); // remember pitch is logarthimic
                else oldfreq += portamento_inc;
                if (oldfreq == newfreq) {
                    algined = true;
                    oldfreq = newfreq;
                }
            }
        }
    
    - Add Mono Mode - selectable at start up.

    - Long button functions (Pages/Shift, LFO/?, Arp/?, Preset/Save) - chosen functions.

    - Start-up settings (MIDI channel, other funtions?)

    - Firmware upgrade procedure (hold reset button and connect to PC/Mac, drag and drop firmware) - Need to have a different name come up


Changelog: 

    12/08/2023:- Added Changelog and Updated synth name.


Things already implemented:

    + Proven hardware functions (Pots, LEDs, Keys, Audio)
        + USB-MIDI
            + Finally added and tested
        + EEPROM
            + Improved Save handling, moved code preset saving code into here to make the UI more friendly:- Rewrite lower storgae code to use PRESET struct instead of breaking it down into bytes (was for an issue due to page size I believe)
        + LEDs
            + Test script for LEDs
                + Added function to show led test on startup if Preset button is held down
        + Buttons
        + Keys

    + Oscillator bug fixes:
        + Added soft clip controls
        + Slighty improved tuning - There was anoticable drift in tuning between octaves/pitch shifts... I've imporved note code to be Q16 to improve accuracy, but still issues.
        + Added soft clipping with adjustable (yet to be assigned) gain control.
        + Added functions for all software controls (mod params)
        + Added a softstart to the Oscillator code - stops it popping when turned on
        + Add functions for all hardware controls
        + Add and test functions for updating parameters, instead of accesing them directly from outside
        + Sample peaking before output - down to the poor implementation of the default C signed/unsigned recasting. 

    + Note Handling:
        + Bugfix: An error with the release message queue was causing the notes not to release properly. Only recognised during Sustain Pedal Feature testing.
        + Moved Note_priority back to HW core - Takes the time pressiure off the Audio core. Note assignments are now sent via a queue. 
        + Added a the bones of a paraphonic filter for Modulation and/or Filter. This is currently unstable, but will possibly be used in future pending some UI testing.

    + Hardware files bug fixes:
        - Bug: Fixed once I moved the note handling to the HW core and refactored the Note Priority code to remove the massive, now unnecessary for-loop. Fast movements can be missed on the pots. at frist I thought it was only down to multiple controls being active, but it seems to also randomly happen on preset 8. Example:- on preset 8, moving the decay really fast misses/looses the control. 
        + Added a basic debug test function to test the pots and cycle for LEDs on start up - can currently be accessed by holding down preset/shift while powering up.
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

    + Settings Bugfixes:
        + Added starting shift functions:
            - Added in functions for Modulation  
            - Added in functions for Filter (Shift on LFO) and its ADSR (Shift on ADSR) - fixed issues with processing.
        + Issues when first starting, LFO was active but UI didnt reflect it.
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
        + Added UART code and associated MIDI call functions.
        + Bugfix: MIDI messages were being repeatedly called, turned out to be and issue with the way that the USB-MIDI queue was being checked for messages. Only found if during testing for Sustain Pedal Feature.
        + Bugfix: MIDI in note calls would fire if keybaord hadnt been pressed. changed the way they're called.
        + MIDI Clock can handle both Clock stopping and Clock drop out (Not ideal, but should be ok for most uses)
        + MIDI Clock can recieve any tempo in the range of 20-999 BPM
        + Updated USB-MIDI data collection. It now checks how many message there are and collects all of them from the TinyUSB MIDI buffer every time round. This is to improve the jitter of the MIDI Clock, but is also good practice.
        + Added and refined MIDI Clock sync
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

    + Filter:
        + Improved Envelope output for highpass control of cutoff - it now applies the envlope downwards.
        + Reduced the sample rate of the filter ADSR. 
        + Added a modulation input, but needs adjusting. Going on the back burner till the Filter and Mod can opperate together.
        + Fixed the inputs using the map_exp functions - now using LUT for speed.
        + Bugfix: fixed the inputs for controls (Cutoff, Resonance, Type, Punch):- needed the ranges fixing but now can happily take 10bit controls. 
        + Added ADSR functions.
        + Controls are now all there (Cutoff, Resonance, Punch, Type) currently accessable by holding shift on the main page.
        + Started adding controls for inputs.
        + Filter is now functional!
        + Added a rough working filter! Didn't think it was possible, so very excited. Thanks to pichenettes. 

