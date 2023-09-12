# Wave-Machine-Firmware

Current nightly firmware for Wave Machine Hardware.


- Release bugfixes

    - OSCILLATORS: Investigate why notes that have been pitched up and octaved up alias/pitch wrongly. Something to do with the overflow of the accumulator, I'm sure of it.

    - NOTE PRIORITY: Investigate moving MIDI::sendNoteOff to outside the note validation loop in release so it always gets called when it actually gets called. Might need to move sustain to keys file...

    - TODO: De-link Latch and Sustain in in Arp - Should lead to better understanding of sustain handling between presets.






- Updates and Bugfixes:

    - Arp:
        - Improvements: Improve the gap (gate?) setting so it's not just on or off. 
            - Bring back the old intermediate NoteState
            - Add a timer function to clock.

        - Bug: MIDI clock freaks out when theres both MIDI and USB-MIDI - more of a MIDI specification problem in general, but will write MIDI message checker to check messages from UART against USB to stop duplicates. 


    - Controls:
        - Change the layout of controls:
            - 1: OSC, 2: LFO, 3: FLT, ALL(4): ARP(For Now)
            - Holding Page should be Shift - this should expose extra features

            - Holding Preset and page should save preset
 
        - Develop way of exporting Presets (probably needs to be linked in to either MIDI or, better yet, some kind of USB mounted storage)


    - Oscillator:
        - Improvement: Finesse soft start code - currently takes too long to get going and still isnt perfect.

        - Bug: Issues with sample generation. - easy fix is to lower the sample rate.
            - This can be demonstrated in Preset 2 with any notes playing when it starts to get to the top of its modulation cycle it glitches out if certain controls are active (most noticeable on MOD or Arp BPM)...
            - Not DMA related
            - Still happens in "stable" Version (main), but maybe less?
            - Seems to be mostly connected to modulating Vector.
            - Happens regardless of wavetable Interpolation, but maybe slightly less without it
            - Only happens with Arp on (test whether its just on fast modes or not, my assumption would be that it is)
            : Could be due to the need to syncronise control updates between cores, make sure they happen outside of DMA / sample creation.


    - Mod:
        - Bug: Vibrato isnt even in +/- (due to the logarithmic nature of pitch) - Fine at >> 8 (+/-40c, but slightly uneven) but over that becomes noticably uneven.
        
        - Feature: Once LFO is balanced, add a MAX_RANGE config asetting for vibrato. 
        - Feature: Add a smooth function (interpolation). This would be helpful for the S&H wave, but also to smooth out the steps in really long wavelengths. - use similar code from normal wavetable.


    - USB MIDI/ MIDI:
        - Bug: When a Stop message via USB-MIDI, the message isn't handled properly in the TinyUSB implementaion cauing the message to be appended with half of the next message (FC and BC 7B 00 become FC BC 7B and 00 00 00)

        - Feature: Handle Song Position messages in a meaningful way - Use them to make sure the arp always starts at the top of the bar...

        - Tidy up MIDI processing code, could be more efficient.
        - Map more CC values and check they're working. 
        
        - Add MIDI channel assignment to start up
        - Add a MIDI settings structure and a way of saving persistant data (via EEPROM)
        
            ? Bug: When switch sync modes in from USB to UART, MIDI messages get jumbled - Pitch bend gets shifted down, when no pitchbend messages should be being sent. Think this is an Ableton issue, but only happens with my synth.


    - Clock: 
        - Feature: Add MIDI Clock out - Will be achieved by dividing internal clock to midi message.

        - Bug: Add dynamic setting of MIDI CLOCK timeout - Currently set to longest possible time out (670000µs for minimum pulse at 20BPM)... just need some kind of calculation so that you get a rough average of say like 8 or 10 pusles + 1000µs?



    - EEPROM:
        - Add factory calibration routine:
            - Fetch Unique ID and store it? (kinda not needed as it's always there, but maybe) - could even be the ID of my memory chip?
        - Add Storage routines for system settings:    
            - Organise a way of storing all those settings in config, and being able to adjust them and re save them.
            - Need a way of storing all settings/config related stuff persistanly (MIDI settings, Audio Engine settings, calibration?, etc.)






- Future Implementaions and WIPs:

    - Add hardware controls for:
        - Factory Reset


    - Improve ADSR code:
        - Try to make the code more portable - currently the calculations for envelope times are done in the controling code to allow for multicore use, but check to see if it can be done in other ways.


    - Improve Mod code:
        - Research: Look up Attenuveter and see if its any use for controlling LFO code.
        - Try making it Poly - I think the sample rate can be reduced by 8 (6kHz) and that should allow every voice to have its own poly Mod. Would probably need a reset for when notes are released, so that you can really hear the difference.
        - Add a tempo sync function.
        - Add ADSR? Maybe share with filter...
        - Add a ramp down feature when switching between destinations - could be difficult. 


    - Improve Filter code: 
        - Check ADSR setup in Filter - Seems to not Update Sustain, might need to retrigger DECAY if sustain is changed, this should make it recalculate sustain level.
        - Improve modulation inputs - Cutoff mod could be applied with a switch [if (Filter::HighCut) 65535 - mod;]


    - Improve Arp code:
        - Improvement: Add a control for Chord Arp
        - Improvement: Add a control for Mono/Para Filter modes
        - Feature: Add a setting for patterns - so that its not just straight Quarter/Sixteenth notes etc. Think 90's/00's timberland synths
        - Feature: Add a swing feature.
        - Feature: Add back Chord Arp - will work great with patterns too.
        - Feature: Add proper Latch/Hold - add a timer within sustain that trakcs us/ms/tick, it removes the last lot and adds the new lot. Probably like the button timer.
        - Feature: Add a function to quantize the Arp. could be that notes don't get updated until the counter resets, or just the next time the beat has changed.

        - Bug: To do with Hold diverting add notes to only refresh - With Hold/Latch engaged (only): If you play a 2 octave C7, followed by a 2 oct Dm7, fine, but if you then play another 2 octave C7, the note organised gets confused. Something to do with the return on double notes I believe... mayeb move the reorganizing to the end of the Note Priority Update loop.


    - Improve Note Handling:
        - Bug: Held notes are being written over desipte some that some voices should be in release - Hold low octave on MIDI, and play fast pentatonic up and down

        - Add actual Mono Mode - selectable at start up.
        
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
    

    - Start-up settings (MIDI channel, other funtions?)


    - Firmware upgrade procedure (hold reset button and connect to PC/Mac, drag and drop firmware) - Need to have a different name come up


    - Test hardware function ideas:
        - CV?






Changelog: 

    25/08/2023:- Added a serial window on startup that tells you all the synth details (Unique ID, Firmware Version, and Core Temp).
    24/08/2023:- Added the Sustaion pedal function.
    12/08/2023:- Added Changelog and Updated synth name.






Features/Bugfixes:

    + Oscillator:
        + Improvement: Added interpolation of wavetable samples for improved tuning.
        + Improvement: Made some improvements and refactored Synth code for clarity. 
        + Bugfix: Frequency/MIDI note alignment - Internal oscillator engine was out of tune with MIDI defined pitches. When Pitch control is central and Octave at 0, the first note on the keyboard is MIDI note 60 (C3/130Hz).
        + Removed all 64bit recasts
        + Added soft clip controls
        + Slighty improved tuning - There was anoticable drift in tuning between octaves/pitch shifts... I've imporved note code to be Q16 to improve accuracy, but still issues.
        + Added soft clipping with adjustable (yet to be assigned) gain control.
        + Added functions for all software controls (mod params)
        + Added a softstart to the Oscillator code - stops it popping when turned on
        + Add functions for all hardware controls
        + Add and test functions for updating parameters, instead of accesing them directly from outside
        + Sample peaking before output - down to the poor implementation of the default C signed/unsigned recasting. 

    + Note Handling:
        + Bugfix: Notes that were held down or in sustain were acting strangely across presets. 
        + Bugfix: Filter triggers didn't count active notes right. 
        + Moved filter trigger code to the Audio core to reduce queue messages.
        + Improvement: Moved Note message handling away from DMA into main loop for improved performance.
        + Feature: Added code to handle the use of an external Sustain Pedal via MIDI as well as rewriting the code for how the Hold fucntion works in the Arp, and having the Sustain Pedal work that too. 
        + Bugfix: An error with the release message queue was causing the notes not to release properly. Only recognised during Sustain Pedal Feature testing.
        + Moved Note_priority back to HW core - Takes the time pressure off the Audio core. Note assignments are now sent via a queue. 
        + Added a the bones of a paraphonic filter for Modulation and/or Filter. This is currently unstable, but will possibly be used in future pending some UI testing.

    + Firmware:
        + Added a dynamic version declaration in the CMake file.

    + Hardware:
        + MIDI:
            + Proven MIDI IN.
            + Proven MIDI OUT.
            + Added code to interface the UART.
        + Bugfix: Fast movements were missed on the controls - Fixed once I moved the note handling to the HW core and refactored the Note Priority code to remove the massive, now unnecessary for-loop.
        + Added a basic debug test function to test the pots and cycle for LEDs on start up - can currently be accessed by holding down preset/shift while powering up.
        + Create a better abstraction layer between the hardware and the software (synth) - currently theres issues passing hardware avriables to the software variables... ADSR/pitch. will also allow for better multicore support
        + USB-MIDI
            + Improved the handling of USB-MIDI notes and there Update calls.
            + Finally added and tested
        + EEPROM
            + Improved Save handling, moved code preset saving code into here to make the UI more friendly:- Rewrite lower storgae code to use PRESET struct instead of breaking it down into bytes (was for an issue due to page size I believe)
        + LEDs
            + Test script for LEDs
                + Added function to show led test on startup if Preset button is held down
        + Buttons
        + Keys
        + DAC

    + Arp:
        + Feature: Added a setting for Chord Arp along with its associated functions.
        + Bugfix: Arp was getting stuck at top or bottom of range. Reworked Octave direction code, aswell as setDirection code.
        + Feature: Added a new Arp mode! Octave Arp:- Plays all notes simultaneously up to POLYPHONY. If the Range setting is set more than 0, it will also Octavate in the same direction as the normal arp is set.
        + Bugfix: The latching code now exsists outside of the sustain function, this should be better for future.
        + Improvement: Quantized the adding and removal of notes to the Arp. The allows for a much better playing experience, along with better midi syncing. It's fun! Will add a setting for this in future, as I'm sure some people wouldnt want it. 
        + Improvements and Bugfix: Added proper Sustain pedal behaviour to the Arp. this now allows a choice of either Momentary Sustain/Latch control, or a toggling behaviour, which works a little better for latch.
        + Bugfix: When changing Presets, the Hold/Latch function wasn't acting correctly and would have hold on if a previous Preset had it enabled.
        + Bugfix: Latch wasn't releasing via MIDI properly. Happened when lost of notes were being pressed, especially when one was still held down.
        + Bugfix: Notes and releases were getting stuck across preset changes.
        + Bugfix: Latching was not releasing old notes when more than polyphony were played.
        + Bigfix: Hold enabling when engaging Arp.
        + Bugfix: State change algorithm rewritten to allow better handling of voice and MIDI notes across state changes. 
        + Feature: Added Sustain pedal code for arp.
        + Feature: Added hardware control for internal BPM speed.
        + Change: Altered how the voice allocation is handled in Arp. Removes the need for note priority, and should allow for better control of Hold/Sustain.
        + Feature: Added a hardware control for Arp Gap. Basically gives you a choice between notes that lead straight into each other adn notes that have a gap between the same duration as the note duration.
        + Bugfix: Arp Sync now working properly. A Bug was highlighted when a bug in the MIDI handling code was fixed, the speed didnt align with the MIDI clock speed.
        + Bugfix: fixed bug in Arp noter removal logic that cause strange behaviour and a comile warning.
        + Arp can't keep up if at high speeds (above 1/16, or 1/32)... ONLY while on arp page:- MUST but the Update of the controls is causing an issue, need to add multicore mailbox/greater issue of unstable controls... 
        + When the range is set to anything above 0 only the first octave of the arp has proper release - think it's to do with the note_clear function in the oscillator (definitely was)
        + fix control for preset/pagination - currently persists between presets no matter of the state of the new preset.
        + currently wont play only one note...
        + Needs direction functionality
        + currently has a random low note on release of arp (noticable in high octaves)
        + Add Arp mode

    + Settings/Controls:
        + Bugfix: Page light wasn't acting correctly on start up.
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

    + ADSR:
        + Fixed an overflow issue with multiple notes being in an extended Decay phase causing the output sample to be limitied cause audio artifacts:- Bug: Clipping output signal - When running ARP full speed/range/DOWN-UP with 8 notes, ADSR attack min, release max, and then start turning decay up, it hard clips... think this is down to attack getting to full value (0xffff) instead of (0x6fff/0x7fff), so when all voices push that mid 16bit in value, the whole output sample volume breaks 16 bit, and therefore clips before getting downsampled.)
        + Removed any useless calls to synth voice stuff so can be used more universally (currently planning on adding to Mod) - currently get passed values for notes and stuff, kinda unnecessary for actual ADSR, but used in this implementaion to clear the voice, could just be donw by checking but I thought it was stopping code. could just use "if (ADSR::isStopped()) Voice::note_clear" in the saudio process code.
        + ADSR not working for first oscillator/voice - added a minimum (10ms) limit on the AD settings... seemed to help. 
        + Improved ADSR - some confusion if you release key in attack stage, skips DS and jumps to release - this is standard behaviour for most synths by testing some.

    + Mod:
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
        + Mod is now opperated by the Synth code -> Move Update closer to synth/dac code:- could probably do with being intergrated like ADSR 
        + Fix control for preset/pagination
        + Make outside variable updates go through functions
        + Make every function take 0-1023 for consistancy from the hardware layer

    + Multicore:
        + Setup Note_Priority tracking system so that it can be moved back to core1
        + Setup referenced function calls for Synth/Mod/Arp controls (seemed to work way better than using the mailbox system and then calling functions on the other side)
        + Added Mailbox for note handling
        + Created/reorganise layer between hardware and synth.
        + Finally added Multicore support (hadware functions on one side, synth/dac on another)

    + MIDI:
        + Moved MIDI clock tracking to a sample based time instead of blocking MCU function.
        + Bugfix: Several bugs were caused by the UART MIDI implementation not formating messages correctly. This has been fixed by a major rewrite of the UART MIDI parsing code, along with an update to the MIDI message handling code.
        + Improved MIDI handling logic by moving the MIDI Channel verification earlier.
        + Bugfix: UART MIDI IN implentation cause false notes to be called.
        + Added UART MIDI IN.
        + Added UART MIDI OUT.
        + Bugfix: MIDI note on/off calls rewritten - the way in which MIDI not on/off calls were coming after note priority/arppegiator was causing a bug where no note off message would be sent if more than 8 voices were played. The update stops this from happening and also allows the sustain funtion to work properly.
        + Bugfix: MIDI IN CC calls no longer get stuck due to controls being live
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
        + Feature: Added a setting for Mono/Para Filter modes - just needs a control now.
        + Bugfix: Filter triggers didn't count active notes right.
        + Moved filter trigger code to the Audio core to reduce queue messages.
        + Added a switch for direction of envelope.
        + Bugfix: Filter now releases properly (depending on mode). Was underflowing when adding more notes than polyphony then releasing.
        + Bugfix - Arp & Filter/Mod Envelope now have an option to be MONO or POLY, this allows the envelope to open slowly up on a playing Arp, or fire for every note.
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

