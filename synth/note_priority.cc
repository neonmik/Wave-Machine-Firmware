#include "note_priority.h"


#include "synth.h"

#include "../midi.h"

namespace NOTE_PRIORITY {

  voice_data_t VOICES[MAX_VOICES];

  // Synth Note Control
  void voice_on(int slot, int note, int velocity) {
    if (note) {

      VOICES[slot].on(note);

      voices_inc();
      filter_on();

      MIDI::sendNoteOn(VOICES[slot].note, velocity);
      QUEUE::trigger_send(slot, VOICES[slot].note, VOICES[slot].gate);
    } else {
      return;
    }
  }
  void voice_off(int slot, int note, int velocity) {

    VOICES[slot].off();

    voices_dec();
    filter_off();

    MIDI::sendNoteOff(VOICES[slot].note, velocity);
    QUEUE::trigger_send(slot, VOICES[slot].note, VOICES[slot].gate);
  }

  void voices_panic() {
    for (int i = 0; i < MAX_VOICES; i++) {
      voice_off(i, 0, 0);
    }
  }

  void filter_on(void) {
    if (voices_active()) { // re-triggers on every new note - needs reworking to allow releasing multiple notes
      // MOD::trigger_attack();
      // FILTER::trigger_attack();
      QUEUE::trigger_send(MAX_VOICES+1, 0, true);
    }
  }
  void filter_off(void) {
    if (!voices_active()) {
      // MOD::trigger_attack();
      // FILTER::trigger_release();
      QUEUE::trigger_send(MAX_VOICES+1, 0, false);
    }
  }

  // Priority Control
  void priority(int status, int note, int velocity) {
    // Note Priority system
    switch (status)  { //check which type we received
      case 0x90:
        if (velocity>0)  {   //is velocity 0?  if so, we want it to be thought of as note off
          volatile int8_t voice = -1; // means if no free voices are left, it will be -1 still

          for (int i = 0; i < MAX_VOICES; i++)  {
            if (VOICES[i].note == note && VOICES[i].gate) { 
              voice = i;
              break;  // breaks for loop as a free slot has been found
            }
            if (!VOICES[i].active) {
              voice = i;
              break;
            }
          }

          // should skip this is a free voice is found
          if (voice<0) {
            int8_t priority_voice = -1;
            volatile uint32_t time_now = to_ms_since_boot(get_absolute_time());
            
            switch (_priority) {
              // When there are no free voices on the synth, this code uses various priority types to assign the new note to a voice.
              case Priority::LAST: {
                // This mode assigns the new note to the voice with the oldest persisting voice (The Latest notes played have prioirty)
                uint32_t longest_released_time = time_now;
                uint32_t longest_active_time = time_now;
                for (int i = 0; i < MAX_VOICES; i++)  {
                  if (!VOICES[i].gate && (VOICES[i].activation_time<longest_released_time)) { // released notes
                    longest_released_time = VOICES[i].activation_time;
                    voice = i; // shouldn't be called unless theres one or more notes in release, and then should give the oldest
                  }
                  else if (VOICES[i].activation_time<longest_active_time) { // still active
                    longest_active_time = VOICES[i].activation_time;
                    priority_voice = i; // will give the oldest voice thats still being used
                  }
                }
                break; 
              }
              case Priority::FIRST: {
                // This mode assigns the new note to the newest persisting voice (the first notes played have priority)
                uint32_t shortest_released_time = 0;
                uint32_t shortest_active_time = 0;
                for (int i = 0; i < MAX_VOICES; i++)  {
                  if (!VOICES[i].active && (VOICES[i].activation_time>shortest_released_time)) { // released notes
                    shortest_released_time = VOICES[i].activation_time;
                    voice = i; // shouldn't be called unless theres one or more notes in release, and then should give the oldest
                  }
                  else if (VOICES[i].activation_time>shortest_active_time) { // still active
                    shortest_active_time = VOICES[i].activation_time;
                    priority_voice = i; // will give the oldest slot thats still being used
                  }
                }
                break;
              }
              case Priority::LOWEST: {
                  // This mode assigns the new note only if the new note is lower than one of the persisting notes (the lowest notes have priority)
                  uint8_t lowest_note = 127; // Initialize to the highest possible MIDI note value
                  for (int i = 0; i < MAX_VOICES; i++) {
                      if (VOICES[i].active && VOICES[i].note < lowest_note) {
                          lowest_note = VOICES[i].note;
                          voice = i; // Assign the slot with the lowest note
                      }
                  }
                  break;
              }
              case Priority::HIGHEST: {
                  // This mode assigns the new note only if the new note is higher than one of the persisting notes (the highest notes have priority)
                  uint8_t highest_note = 0; // Initialize to the lowest possible MIDI note value
                  for (int i = 0; i < MAX_VOICES; i++) {
                      if (VOICES[i].active && VOICES[i].note > highest_note) {
                          highest_note = VOICES[i].note;
                          voice = i; // Assign the slot with the highest note
                      }
                  }
                  break;
              }
            }
            // No slots in release? Use the next priority appropriate active voice
            if (voice < 0) {
              voice = priority_voice;
            }
          }
          voice_on(voice, note, velocity);
          
          break;  //only breaks if velocity > 0.  otherwise, the switch just rolls onto next case, note_off.
        }

      case 0x80:
        for (int8_t voice = 0; voice < MAX_VOICES; voice++)  {
          if (VOICES[voice].note == note)  { //check for a matching note
            voice_off(voice, note, velocity);
            //no break here just in case there are somehow multiple of the same note stuck on
          }
        }
        break;
      default:
        break;
    }
  }

  void check_release () {
    // loops through the available queue entries till it's empty.
    uint8_t queue_level = QUEUE::release_check_queue();
    for (int i = 0; i > queue_level; i++) {
      // receives the slot number thats released from the queue, and then clears the slot on this core.
      uint8_t slot = QUEUE::release_receive();
      VOICES[slot].clear();
    }
  }
  // transfer from Key/Midi notes to Arp/Note Priority
  void update() {

    // update voice info - used to pull info from other core
    check_release(); // accesses safe data to check notes are free and done releasing
    
    // grab values from notes mutex
    // bool arpEnabled = ARP::get_state(); // Store the value of ARP::get() in a variable

    // testing removal - trying to reduce big for loops to improve code. this chunk is a hang over from the old multicore handling.
    // for (int i = 0; i < 128; i++) {
    //   if (_note_state[i] != _note_state_last[i]) {
    //     if (!arpEnabled) {
    //       if (_note_state[i]) {
    //         priority(0x90, i, 127); // synth voice allocation
    //       } if (!_note_state[i]) {
    //         priority(0x80, i, 0); // synth voice allocation
    //       }
    //     } else {
    //       if (_note_state[i]) {
    //         ARP::add_notes(i);
    //       } else {
    //         ARP::remove_notes(i);
    //       }
    //     }
    //     _note_state_last[i] = _note_state[i];
    //   }
    // }

    if (ARP::get_state()) { 
      ARP::organise_notes();
      ARP::update();
    }


  }

  void note_on (uint8_t note, uint8_t velocity) {
      // _note_state[note & 127] = 1;
      if (!ARP::get_state()) {
        priority(0x90, note, velocity); // synth voice allocation
      } else {
        ARP::add_notes(note);
      }
      _notes_on++;
  }
  void note_off (uint8_t note, uint8_t velocity) {
      // _note_state[note & 127] = 0;
      if (!ARP::get_state()) {
        priority(0x80, note, velocity); // synth voice allocation
      } else {
        ARP::remove_notes(note);
      }
      _notes_on--;
  }
  void notes_clear (void) {
      for (int i = 0; i < 128; i++) {
      _note_state[i] = 0;
      }
      _notes_on = 0;
  }
  uint8_t get_notes_on (void) {
      return _notes_on;
  }
}