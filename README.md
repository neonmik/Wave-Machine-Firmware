# Wave-Machine-Firmware

This is a project for firmware on a custom PCB. This wont work on other systems, is likely unstable, and likely to change. Use any code at your own risk. 


Current nightly firmware for Wave Machine Hardware.

- Features:

    - Powerful 8 Voice Wavetable synth engine with:
        - Polyphonic Amp envelope generator (ADSR).
        - Polyphonic Wavetable oscillator with waveshape and vector control (OSC1).
        - Polyphonic Sub or Detuned oscillator with seperate waveshape control, detune setting, and level (OSC2).
        - Polyphonic Noise Oscillator with level control.
        - Pitchbend
        - Octave selection.
        - Paraphonic LFO and Filter.

    - A highly useable LFO with:
        - Paraphonic Depth envelope control (ADSR)
        - 6 Waveforms (Sine, Sawtooth Up, Sawtooth Down, Square, Triangle and Noise).
        - A wide range of freqency control from 0.01 - 150Hz (with hardware settings to change to allow for differnt bands in the range of 0.003Hz - 2.4kHz).
        - Depth control.
        - 4 selectable output destinations - Pitch (Vibrato), Volume (Tremelo), Wavetable vector (Allowing for sweeping morphing sounds) and Filter Cutoff modulation.

    - A characterful SVF Filter with:
        - Paraphonic Cutoff envelope control (ADSR) with either ascending or decending modes.
        - Cutoff control
        - Resonance control
        - Punch contol
        - Low-pass, Bandpass and High-pass modes.

    - Fully fledged arppegiator with:
        - Selectable modes (UP/DOWN/DOWN-UP/UP-DOWN and Chord mode)
        - A 1-4 octave range in each mode.
        - A Latch function that allow the dynamic playing of chords.
        - Selectable division from 1/1 to 1/64t.
        - Internal BPM selection (30 - 350 BPM) or MIDI sync'd.
        - Gate setting for the duration of each individual note in the arp sequence.

    - 8 selectable Presets with a colour LED showing current Preset, along with a persisting EEPROM memory storage allowing to save your own presets.

    - MIDI in and out with CC and midi sync (currently only sync from external source, can't provide sync out yet).

    - Hardware:
        - Tactile 27 key keyboard
        - 4 playable knobs
        - Volume Control
        - Currently USB powered, but eventually will take a battery or DC connection.
        - MIDI DIN and USB MIDI
        - 12-bit 32khz Mono 1/4" jack output.

    - New features are currently being added daily.



- Release bugfixes

    - NOTE PRIORITY: Investigate moving MIDI::sendNoteOff to outside the note validation loop in release so it always gets called when it actually gets called. Might need to move sustain to keys file...



- Updates and Bugfixes:

    - Arp:
        - Notes: The transfer/update of notes should check to see if any of the currently pressed niotes are already active and move the pointer around to match these to stop things getting messed up between chord changes?
        
        - Bug: The adding and removing of notes still feels slightly wrong... especially in multiple octave range arps. Investigate possible improvements.
        - Bug: Fix New style Arp Mode (call it "Classic" after the JUNO method). Currently doesn't work in UP/DOWN or DOWN/UP modes due to the octave not moving octav when at the top correctly.
        - Bug: When using DOWN/UP or UP/DOWN with a range greater than 1, the first note after the octave change doesnt start on the first note, but the second.  


    - Controls:
        
        - Change the layout of controls:

            - Page/Shift Button:
                - Pressing this should cycle between pages:
                    - 1: OSC, 2: LFO, 3: FLT, ALL(4): ARP(For Now)
                - Holding this is SHIFT: 
                    - This currently exposes the envelope pages for OSC/FLT pages, and extra functions on LFO/ARP pages. It also exposes various functions to buttons.

            - Button 1 [Old Mod Button]:
                - Pressing this toggles LFO on/off
                - Pressing this with Shift toggles FLT on/off.
                - Holding this does nothing.
                - Holding this with Shift does nothing.

            - Button 2 [Old Arp Button]:
                - Pressing this toggles on/off Arp
                - Pressing this with Shift toggles Latch on/off.
                - Holding this does nothing.
                - Holding this with Shift does nothing.

            - Preset Button:
                - Pressing this cycles between presets 1-8
                - Pressing this with shift Saves the current preset to EEPROM.
 
        - Develop way of exporting Presets (probably needs to be linked in to either MIDI or, better yet, some kind of USB mounted storage)


    - Oscillator:
        - Improvement: Look into adding an extra 8 bit variable to count the roll over of the Phase Accumulator to stop the frequency overflowing at the top of the octave/pitch bend range (currently octave 3, with pitchbend above 1/3 will wrap round and be the lowest note). - it may be better to change octave to -2, -1, 0, +1, +2 instead.

        - Improvement: Finesse soft start code - currently removed as it took too long to get going and still didnt realy work.

    - Mod:
        - Improvement: Vibrato isnt even in +/- (due to the logarithmic nature of pitch) - Fine at >> 8 (+/-40c, but slightly uneven) but over that becomes noticably uneven.
        
        - Feature: Once LFO is balanced, add a MAX_RANGE config setting for vibrato. 

        - Feature: Add a smooth function (interpolation). This would be helpful for the S&H wave, but also to smooth out the steps in really long wavelengths. - use similar code from normal wavetable.


    - USB MIDI/ MIDI:
        - Bug: when pressing and releasing notes, many Note Off messages seem to be sent.
        
        - Bug: When a Stop message via USB-MIDI, the message isn't handled properly in the TinyUSB implementaion cauing the message to be appended with half of the next message (FC and BC 7B 00 become FC BC 7B and 00 00 00)

        - Bug: MIDI clock freaks out when theres both MIDI and USB-MIDI - more of a MIDI specification problem in general, but will write MIDI message checker to check messages from UART against USB to stop duplicates. 

        - Feature: Handle Song Position messages in a meaningful way - Use them to make sure the arp always starts at the top of the bar...

        - Tidy up MIDI processing code, could be more efficient.
        - Map more CC values and check they're working. 
        
        - Add MIDI channel assignment to start up
        - Add a MIDI settings structure and a way of saving persistant data (via EEPROM)
        
            ? Bug: When switch sync modes in from USB to UART, MIDI messages get jumbled - Pitch bend gets shifted down, when no pitchbend messages should be being sent. Think this is an Ableton issue, but only happens with my synth.


    - Clock: 
        - Improvement: Add start and stop messages

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
        - Feature: Add infinite release mode - Once pot is at full rotation, lock out release allowing note to sustain forever.


    - Improve Mod code:
        - Try making it Poly - I think the sample rate can be reduced by 8 (6kHz) and that should allow every voice to have its own poly Mod. Would probably need a reset for when notes are released, so that you can really hear the difference.
        
        - Add a tempo sync function.
        
        - Add a ramp down feature when switching between destinations - could be difficult. 

        - Research: Look up Attenuveter and see if its any use for controlling LFO code.


    - Improve Filter code: 

        - Check ADSR setup in Filter - Seems to not update Sustain, might need to retrigger DECAY if sustain is changed, this should make it recalculate sustain level.
        - Improve modulation inputs - Cutoff mod could be applied with a switch [if (Filter::HighCut) 65535 - mod;]


    - Improve Arp code:

        - Improvement: Add a control for Mono/Para Filter modes
        - Feature: Add a setting for patterns - so that its not just straight Quarter/Sixteenth notes etc. Think 90's/00's timberland synths
        - Feature: Add a swing feature.
        - Feature: Add back Chord Arp - will work great with patterns too.
        
        - Feature: Add a function to quantize the Arp. could be that notes don't get updated until the counter resets, or just the next time the beat has changed.

        - Bug: To do with Hold diverting add notes to only refresh - With Hold/Latch engaged (only): If you play a 2 octave C7, followed by a 2 oct Dm7, fine, but if you then play another 2 octave C7, the note organised gets confused. Something to do with the return on double notes I believe... mayeb move the reorganizing to the end of the Note Priority update loop.


    - Improve Note Handling:
        - Bug: When sustain is held, note priority overtakes still held sustained note.Need to add some validity check for (if (sustainPedal) if )

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
        + Feature: Add Sub and Noise oscillators to the synth engine, along with temporary controls for off/on. 
        + Bugfix: Fixed most of the aliasing that came with having the octave and pitch set to max. 
        + Improvement: Fixed wavetable tuning. Implemented by reviewing and refactoring the accumulator and interpolation code. Massive win. 
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
        + Bugfix: Held notes were being written over desipte the fact that some voices should be in release (Could be demo'd by holding low octave on MIDI, and play fast pentatonic up and down). Can no longer be replicated, must have been fixed when I refactored Note Handling.
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
            + Improved the handling of USB-MIDI notes and there update calls.
            + Finally added and tested
        + EEPROM
            + Feature: Added storage locations for Presets, Factory Presets, and System settings. Have given provision for 8 banks of 8 presets (64 Presets) at 2 pages each (128 bytes).
            + Improvement: Refactored EEPROM Preset storage code to dirctly handle PRESET struct for neater handling of preset saving/loading.
            + Improved Save handling, moved code preset saving code into here to make the UI more friendly.
        + LEDs
            + Bugfix: RGB toggle wasn't in the correct oreintation on exit - this was due to an issue with how colour settings were handled.
            + Improvement: Test function and flashDelay/Cycle functions now handle delay parameters properly. 
            + Test script for LEDs
                + Added function to show led test on startup if Preset button is held down
        + ADC
            + Bugfix: ADC figures were not scanned properly on startup - rearranged startup controls and added a delay to stop race conditions that were present after the changes.
        + Buttons
        + Keys
        + DAC

    + Arp: 
        + Bugifx: Fixed a bug that caused notes to hang (MIDI and internal) when flipping between MONO and POLY arp mode. Made sure that the mode was only updated when inside the update loop.
        + Feature: Added a setting for "Played Order". Currently can only be turned on in firmware update, as no controls left to change it.
        + Feature: Added and fully implemented the Gate function. This allows you to use a pot to change the length of the Arp notes. setting fully Anti-Clockwise will result in a very short blip, while setting fully Clockwise will result in a note that ends when the next note begins. Make sure the Filter envelope is set correctly or you wont hear anything! 
        + Bugfix: Fixed a bug where flipping through presets while actively playing (not just holding notes/chords) notes would get stuck sustaining. This was down to notes not being transfer from the Arp input buffer to the play buffer before being copied to the standard note handling code.
        + Improvement: Added a control for Chord Arp which can be accessed from 
        + Feature: Added a setting for Chord Arp along with its associated functions.
        + Bugfix: Arp was getting stuck at top or bottom of range. Reworked Octave direction code, aswell as setDirection code.
        + Feature: Added a new Arp mode! Octave Arp:- Plays all notes simultaneously up to POLYPHONY. If the Range setting is set more than 0, it will also Octavate in the same direction as the normal arp is set.
        + Feature: Added a proper Latch/Hold feature which watches whether anr notes are still being held from the current chord. If no current note being played is held, the latch will release all notes, and start holding the new note/chord.
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
        + Arp can't keep up if at high speeds (above 1/16, or 1/32)... ONLY while on arp page:- MUST but the update of the controls is causing an issue, need to add multicore mailbox/greater issue of unstable controls... 
        + When the range is set to anything above 0 only the first octave of the arp has proper release - think it's to do with the note_clear function in the oscillator (definitely was)
        + fix control for preset/pagination - currently persists between presets no matter of the state of the new preset.
        + currently wont play only one note...
        + Needs direction functionality
        + currently has a random low note on release of arp (noticable in high octaves)
        + Add Arp mode

    + Settings/Controls:
        + Improvement: Added a counter to stop controls up dating too quickly. Was overloading the audio processor, causing glitches and drop outs and was unnecessary.
        + Improvement: Added timeout feature for shift function to stop it jumping too quickly to shift pages.
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
        + Bugfix: Presets are always loaded on start up. Thjis allows the hardware controls and LEDs to be correct.

    + ADSR:
        + Bugfix: By calling the Envelope controls as Static, I can now compile at Release level (-O3 Optimizations were breaking both the old and new ADSR/who synth in general)
        + Improvement: Refactored code to be more modular. Now controls can be perfromaed easier and are potable.
        + Fixed an overflow issue with multiple notes being in an extended Decay phase causing the output sample to be limitied cause audio artifacts:- Bug: Clipping output signal - When running ARP full speed/range/DOWN-UP with 8 notes, ADSR attack min, release max, and then start turning decay up, it hard clips... think this is down to attack getting to full value (0xffff) instead of (0x6fff/0x7fff), so when all voices push that mid 16bit in value, the whole output sample volume breaks 16 bit, and therefore clips before getting downsampled.
        + Removed any useless calls to synth voice stuff so can be used more universally (currently planning on adding to Mod) - currently get passed values for notes and stuff, kinda unnecessary for actual ADSR, but used in this implementaion to clear the voice, could just be donw by checking but I thought it was stopping code. could just use "if (ADSR::isStopped()) Voice::note_clear" in the saudio process code.
        + ADSR not working for first oscillator/voice - added a minimum (10ms) limit on the AD settings... seemed to help. 
        + Improved ADSR - some confusion if you release key in attack stage, skips DS and jumps to release - this is standard behaviour for most synths by testing some.

    + Mod:
        + Improvement: Tidied code to make it more modular.
        + Feature: Added envelope to Mod. Currently not controlable on hardware, but functionality is there.
        + Added a linear to exponential curve for the Rate function. Can now rate between 0.1Hz and 5000Hz with the lower end of the range being finest, and the higher end coarsest.
        + Bugfix: Mod overflowing vector/wavetable index - reworked fault in wavetable.
        + Tidied up code to remove unnecessary stuff.
            + Moved PRNG to its own files - allows it to be used elsewhere.
            + Removed int8_output/uint10_output functions as not needed anymore.
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

    + MIDI:
        + Feature: Added MIDI Clock out. MIDI clock sync message are sent at 24ppqn from the internal BPM clock.
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
        + Bugfix: Filter envelope was reseting randomly (when playing arp with short release and longer attack (either mono or poly) but seemed to be fine with any release). Can't replicate now, so marking as fixed. Added watchdog/debug messages just incase.
        + Feature: Added a setting for Mono/Para Filter modes - just needs a control now.
        + Bugfix: Filter triggers didn't count active notes right.
        + Moved filter trigger code to the Audio core to reduce queue messages.
        + Added a switch for direction of envelope.
        + Bugfix: Filter now releases properly (depending on mode). Was underflowing when adding more notes than polyphony then releasing.
        + Bugfix - Arp & Filter/Mod Envelope now have an option to be MONO or POLY, this allows the envelope to open slowly up on a playing Arp, or fire for every note.
        + Improved Envelope output for highPass control of cutoff - it now applies the envlope downwards.
        + Reduced the sample rate of the filter ADSR. 
        + Added a modulation input, but needs adjusting. Going on the back burner till the Filter and Mod can opperate together.
        + Fixed the inputs using the map_exp functions - now using LUT for speed.
        + Bugfix: fixed the inputs for controls (Cutoff, Resonance, Type, Punch):- needed the ranges fixing but now can happily take 10bit controls. 
        + Added ADSR functions.
        + Controls are now all there (Cutoff, Resonance, Punch, Type) currently accessable by holding shift on the main page.
        + Started adding controls for inputs.
        + Filter is now functional!
        + Added a rough working filter! Didn't think it was possible, so very excited. Thanks to pichenettes. 

