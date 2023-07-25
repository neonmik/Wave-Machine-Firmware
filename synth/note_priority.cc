#include "note_priority.h"


#include "synth.h"

#include "../midi.h"
// #include "modulation.h"

namespace NOTE_PRIORITY {

  voice_data VOICES [8];
  // Synth Note Control
  void voice_on(int slot, int note, int velocity) {
    if (note) {
      // filter_on();
      // for MIDI Out use: - here so that the arp can output MIDI
      // MIDI::sendNoteOn(note, velocity); // Needs reworking for multcore

      SYNTH::voice_on(slot, note, get_freq(note));
    } else {
      return;
    }
  }
  void voice_off(int slot, int note, int velocity) {
    // filter_off();
    // for MIDI Out use: - here so that the arp can output MIDI
    // MIDI::sendNoteOff(note, velocity); // Needs reworking for multcore

    SYNTH::voice_off(slot);
  }
  void voices_panic() {
    for (int i = 0; i < 8; i++) {
      // for MIDI Out use:-
      // MIDI::sendNoteOff(SYNTH::channels[i].note, 0); // Needs reworking for multcore
      SYNTH::voice_off(i);
    }
  }

  void filter_on(void) {
    // for future MOD/Filter ADSR
      ++_voices_active;
      if (_voices_active > 8) {
        _voices_active = 8;
      }
      if (_voices_active) { // re-triggers on every new note - needs reworking to allow releasing multiple notes
        // MOD::trigger_attack();
        FILTER::trigger_attack();
      }
  }
  void filter_off(void) {
    // for future MOD/Filter ADSR
    --_voices_active;
    if (_voices_active <= 0) {
      _voices_active = 0;
      FILTER::trigger_release();
    }

    // printf("Voice Off:      %d\n", slot);
    // printf("Note:           %d\n", note);
    // printf("Voices active:  %d\n", _voices_active);
  }

  // Priority Control
  void priority(int status, int note, int velocity) {
    // Note Priority system
    switch (status)  { //check which type we received
      case 0x90:
        if (velocity>0)  {   //is velocity 0?  if so, we want it to be thought of as note off
          volatile int8_t slot = -1; // means if no free voices are left, it will be -1 still

          for (int i = 0; i < MAX_VOICES; i++)  {
            if (SYNTH::channels[i]._note == note && SYNTH::channels[i]._gate) { 
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
                  if (!SYNTH::channels[i]._gate && (SYNTH::channels[i].activation_time<longest_released_time)) { // released notes
                    longest_released_time = SYNTH::channels[i].activation_time;
                    slot = i; // shouldn't be called unless theres one or more notes in release, and then should give the oldest
                  }
                  else if (SYNTH::channels[i].activation_time<longest_active_time) { // still active
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
                  if (!_active_voice[i] && (_time_activated[i]>shortest_released_time)) { // released notes
                    shortest_released_time = _time_activated[i];
                    slot = i; // shouldn't be called unless theres one or more notes in release, and then should give the oldest
                  }
                  else if (_time_activated[i]>shortest_active_time) { // still active
                    shortest_active_time = _time_activated[i];
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

    // update oscillator info - will be used to send release messages back from other core
    // Voices::Update(); // should access safe data to pull released states

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



// new code
// namespace NOTE_PRIORITY {

//   voice_data VOICES[8];
//   // Synth Note Control
//   void voice_on(int slot, int note, int velocity) {
//     if (note) {
      
//       voices_inc();
//       if (voices_active()) { // re-triggers on every new note - needs reworking to allow releasing multiple notes
//         // MOD::trigger_attack();
//         FILTER::trigger_attack();
//       }

//       MIDI::sendNoteOn(note, velocity);
//       VOICES[slot].voice_on(note, velocity); // new voice handling in hardware core

//     } else {
//       return;
//     }
//   }
//   void voice_off(int slot, int note, int velocity) {
    
//     voices_dec();
//     if (!voices_active()) {
//       // MOD::trigger_release();
//       FILTER::trigger_release();
//     }

//     MIDI::sendNoteOff(note, velocity); // Needs reworking for multcore
//     VOICES[slot].voice_off(note, velocity); // new voice handling in hardware core
//   }

//   void voices_panic() {
//     for (int i = 0; i < 8; i++) {
//       voice_off(i, 0, 0);
//     }
//   }

//   // Priority Control
//   void priority(int status, int note, int velocity) {
//     // Note Priority system
//     switch (status)  { //check which type we received
//       case 0x90:
//         if (velocity>0)  {   //is velocity 0?  if so, we want it to be thought of as note off
//           volatile int8_t slot = -1; // means if no free voices are left, it will be -1 still

//           for (int i = 0; i < MAX_VOICES; i++)  {
//             if (VOICES[i].note == note && VOICES[i].gate) { 
//               slot = i;
//               break;  // breaks for loop as a free slot has been found
//             }
//             if (!VOICES[i].active) {
//               slot = i;
//               break;
//             }
//           }

//           // should skip this is a free voice is found
//           if (slot<0) {
//             int8_t oldest_slot = -1;
//             volatile uint32_t time_now = to_ms_since_boot(get_absolute_time());
            
//             switch (_priority) {
//               // oldest note used first
//               case Priority::LAST: {
//                 uint32_t longest_released_time = time_now;
//                 uint32_t longest_active_time = time_now;
//                 for (int i = 0; i < MAX_VOICES; i++)  {
//                   if (!VOICES[i].gate && (VOICES[i].activation_time<longest_released_time)) { // released notes
//                     longest_released_time = VOICES[i].activation_time;
//                     slot = i; // shouldn't be called unless theres one or more notes in release, and then should give the oldest
//                   }
//                   else if (VOICES[i].activation_time<longest_active_time) { // still active
//                     longest_active_time = VOICES[i].activation_time;
//                     oldest_slot = i; // will give the oldest slot thats still being used
//                   }
//                 }
//                 break; 
//               }
//               case Priority::FIRST: {
//                 uint32_t shortest_released_time = 0;
//                 uint32_t shortest_active_time = 0;
//                 for (int i = 0; i < MAX_VOICES; i++)  {
//                   if (!VOICES[i].active && (VOICES[i].activation_time>shortest_released_time)) { // released notes
//                     shortest_released_time = VOICES[i].activation_time;
//                     slot = i; // shouldn't be called unless theres one or more notes in release, and then should give the oldest
//                   }
//                   else if (VOICES[i].activation_time>shortest_active_time) { // still active
//                     shortest_active_time = VOICES[i].activation_time;
//                     oldest_slot = i; // will give the oldest slot thats still being used
//                   }
//                 }
//                 break;
//               }
//             }
//             // is there still no free slots?
//             if (slot < 0) {
//               slot = oldest_slot; // if theres a note thats been released already, use the oldest one, otherwise, use the longest held note
//             }
//           }
//           voice_on(slot, note, velocity);
          
//           break;  //only breaks if velocity > 0.  otherwise, the switch just rolls onto next case, note_off.
//         }

//       case 0x80:
//         for (int voice = 0; voice < MAX_VOICES; voice++)  {
//           if (VOICES[voice].note == note)  { //check for a matching note
//             voice_off(voice, note, velocity);
//             //no break here just in case there are somehow multiple of the same note stuck on
//           }
//         }
//         break;
//       default:
//         break;
//     }
//   }

//   // transfer from Key/Midi notes to Arp/Note Priority
//   void update() {
//     // grab values from notes mutex
//     bool arpEnabled = ARP::get_state(); // Store the value of ARP::get() in a variable

//     // update oscillator info - will be used to send release messages back from other core
//     // MAILBOX::VOICES.Update(); // should access safe data to pull released states

//     for (int i = 0; i < 128; i++) {
//       if (_note_state[i] != _note_state_last[i]) {
//         if (!arpEnabled) { // Use the stored value of ARP::get()
//           if (_note_state[i]) {
//             priority(0x90, i, 127); // synth voice allocation
//           } else {
//             priority(0x80, i, 0); // synth voice allocation
//           }
//         } else {
//           if (_note_state[i]) {
//             ARP::add_notes(i);
//           } else {
//             ARP::remove_notes(i);
//           }
//         }
//         _note_state_last[i] = _note_state[i];
//       }
//     }

//     if (arpEnabled) { // Use the stored value of ARP::get()
//       ARP::organise_notes();
//       ARP::update();
//     }


//   }

//   void note_on (uint8_t note) {
//       _note_state[note & 127] = 1;
//   }
//   void note_off (uint8_t note) {
//       _note_state[note & 127] = 0;
//   }
//   void notes_clear (void) {
//       for (int i = 0; i < 128; i++) {
//       _note_state[i] = 0;
//       }
//       _notes_on = 0;
//   }
//   uint8_t get_notes_on (void) {
//       return _notes_on;
//   }
// }