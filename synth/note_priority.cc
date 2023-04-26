#include "note_priority.h"


#include "synth.h"


namespace NOTE_PRIORITY {

  // Synth Note Control
  void voice_on(int slot, int note, int velocity) {
    if (note) {
      SYNTH::voice_on(slot, note, get_freq(note));
      _voice_notes[slot] = note;
      _time_activated[slot] = sample_clock;
      
      // for future MOD ADSR
      // ++_voices_active;
      // if (_voices_active > 8) {
      //   _voices_active = 8;
      // }
      // if (_voices_active) MOD::attack();
      
      // for future MIDI out
      // sendNoteOn(note) // put MIDI note out here
    } else {
      return;
    }
  }
  void voice_off(int slot, int note, int velocity) {
    SYNTH::voice_off(slot);
    _voice_notes[slot] = 0;
    _time_activated[slot] = 0;
    
    // for future MOD ADSR
    // --_voices_active;
    // if (_voices_active < 0) {
    //   _voices_active = 0;
    // }
    // if (_voices_active) MOD::release();

    // for future MIDI out
    // sendNoteOff(note) // put MIDI note out here
  }
  void voices_clear() {
    for (int i = 0; i < 8; i++) {
      // sendNoteOff(SYNTH::channels[i].note) // put MIDI note out here
      SYNTH::voice_off(i);
    }
  }

  // Priority Control
  void priority(int status, int note, int velocity) {
    // Note Priority system
    switch (status)  { //check which type we received
      case 0x90:
        if (velocity>0)  {   //is velocity 0?  if so, we want it to be thought of as note off
          volatile int8_t slot = -1; // means if no free voices are left, it will be -1 still

          for (int i = 0; i < MAX_VOICES; i++)  {
            if (SYNTH::channels[i]._note == note && SYNTH::channels[i]._active) { 
              slot = i;
              break;  // breaks for loop as a free slot has been found
            }
            if (!SYNTH::channels[i]._active) {
              slot = i;
              break;
            }
            
          }
          // should skip this is a free voice is found
          if (slot<0) {
            int8_t oldest_slot = -1;
            volatile uint32_t time_now = to_ms_since_boot(get_absolute_time());
            
            switch (_priority) {
              // oldest note used first
              case Priority::LAST: {
                uint32_t longest_released_time = time_now;
                uint32_t longest_active_time = time_now;
                for (int i = 0; i < MAX_VOICES; i++)  {
                  if (!SYNTH::channels[i]._gate && (SYNTH::channels[i].activation_time<longest_released_time)) {
                    longest_released_time = SYNTH::channels[i].activation_time;
                    slot = i; // shouldn't be called unless theres one or more notes in release, and then should give the oldest
                  }
                  //still active
                  else if (SYNTH::channels[i].activation_time<longest_active_time) {
                    longest_active_time = SYNTH::channels[i].activation_time;
                    oldest_slot = i; // will give the oldest slot thats still being used
                  }
                }
                break; 
              }
              case Priority::FIRST: {
                uint32_t shortest_released_time = 0;
                uint32_t shortest_active_time = 0;
                for (int i = 0; i < MAX_VOICES; i++)  {
                  if (!SYNTH::channels[i]._gate && (SYNTH::channels[i].activation_time>shortest_released_time)) {
                    shortest_released_time = SYNTH::channels[i].activation_time;
                    slot = i; // shouldn't be called unless theres one or more notes in release, and then should give the oldest
                  }
                  //still active
                  else if (SYNTH::channels[i].activation_time>shortest_active_time) {
                    shortest_active_time = SYNTH::channels[i].activation_time;
                    oldest_slot = i; // will give the oldest slot thats still being used
                  }
                }
                break;
              }
            }
            // is there still no free slots?
            if (slot < 0) {
              slot = oldest_slot; // if theres a note thats been released already, use the oldest one, otherwise, use the longest held note
            }
          }
          voice_on(slot, note, velocity);
          
          
          break;  //only breaks if velocity > 0.  otherwise, the switch just rolls onto next case, note_off.
        }

      case 0x80:
        for (int voice = 0; voice < MAX_VOICES; voice++)  {
          if (SYNTH::channels[voice]._note == note)  { //check for a matching note
            voice_off(voice, note, velocity);
            //no break here just in case there are somehow multiple of the same note stuck on
          }
        }
        break;
      default:
        break;
    }
  }

  // transfer from Key/Midi notes to Arp/Note Priority
  void update() {
    // grab values from notes mutex
    bool arpEnabled = ARP::get_state(); // Store the value of ARP::get() in a variable

    for (int i = 0; i < 128; i++) {
      if (NOTES.note_state[i] != _note_state_last[i]) {
        if (!arpEnabled) { // Use the stored value of ARP::get()
          if (NOTES.note_state[i]) {
            priority(0x90, i, 127); // synth voice allocation
          } else {
            priority(0x80, i, 0); // synth voice allocation
          }
        } else {
          if (NOTES.note_state[i]) {
            ARP::add_notes(i);
          } else {
            ARP::remove_notes(i);
          }
        }
        _note_state_last[i] = NOTES.note_state[i];
      }
    }

    if (arpEnabled) { // Use the stored value of ARP::get()
      ARP::organise_notes();
      ARP::update();
    }
  }
}

