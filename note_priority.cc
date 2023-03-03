#include "note_priority.h"

#include "synth/synth.h"

namespace Note_Priority {
  void note_on(int slot, int note, int velocity) {

    if (!note) return; //for the ARP? meant to stop the playing of a random note when letting go.
    SYNTH::channels[slot].note_on(note, get_freq(note));
  }
  void note_off(int slot, int note, int velocity) {
    int _slot = slot;
    SYNTH::channels[slot].note_off();
  }

  void event(int status, int note, int velocity) {
    // Last-note priority... add in lowest/highest
    switch (status)  { //check which type we received
      case 0x90:
        if (velocity>0)  {   //is velocity 0?  if so, we want it to be thought of as note off
          int8_t slot = -1; // means if no free voices are left, it will be -1 still

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
            uint32_t time_now = to_ms_since_boot(get_absolute_time());
            
            // oldest note used first
            uint32_t longest_released_time = time_now;
            uint32_t longest_active_time = time_now;
            for (int i = 0; i < MAX_VOICES; i++)  {
              if (!SYNTH::channels[i]._gate && (SYNTH::channels[i].adsr_activation_time<longest_released_time)) {
                longest_released_time = SYNTH::channels[i].adsr_activation_time;
                slot = i; // shouldn't be called unless theres one or more notes in release, and then should give the oldest
              }
              //still active
              else if (SYNTH::channels[i].adsr_activation_time<longest_active_time) {
                longest_active_time = SYNTH::channels[i].adsr_activation_time;
                oldest_slot = i; // will give the oldest slot thats still being used
              }
            }

            // lastest note used first (keeps first held notes intact, and uses the more recent notes)
            // uint32_t longest_released_time = 0; // for keeping track of the slot which has been held the longest
            // uint32_t longest_active_time = 0; // for keeping track of the slot which has been held the longest
            // for (int i = 0; i < MAX_VOICES; i++)  {
            //   if (!SYNTH::channels[i].gate && (SYNTH::channels[i].adsr_activation_time>longest_released_time)) {
            //     longest_released_time = SYNTH::channels[i].adsr_activation_time;
            //     slot = i; // shouldn't be called unless theres one or more notes in release, and then should give the oldest
            //     printf("released slot %u \n", slot);
            //   }
            //   //still active
            //   else if (SYNTH::channels[i].adsr_activation_time>longest_active_time) {
            //     longest_active_time = SYNTH::channels[i].adsr_activation_time;
            //     oldest_slot = i; // will give the oldest slot thats still being used
            //     // printf("finding active slot %u \n", i);
            //   }
            // }


            // is there still no free slots?
            if (slot < 0) {
              slot = oldest_slot; // if theres a note thats been released already, use the oldest one, otherwise, use the longest held note
            }
          }
          note_on(slot, note, velocity);
          
          break;  //only breaks if velocity > 0.  otherwise, the switch just rolls onto next case, note_off.
        }

      case 0x80:
        for (int voice = 0; voice < MAX_VOICES; voice++)  {
          if (SYNTH::channels[voice]._note == note)  { //check for a matching note
            note_off(voice, note, velocity);
            //no break here just in case there are somehow multiple of the same note stuck on
          }
        }
        break;
      default:
        break;
    }
  }
}