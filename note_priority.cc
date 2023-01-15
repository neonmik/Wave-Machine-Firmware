
#define NOTE_DEBUG 0
#define MAX_VOICES 8

int note2freq[] = {8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 19, 21, 22, 23, 24, 26, 28, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902, 8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544};

void set_note_on(int slot, int note, int velocity) {
  synth::channels[slot].note  = note;
  synth::channels[slot].frequency = note2freq[synth::channels[slot].note];
  synth::channels[slot].gate = true;
  synth::channels[slot].is_active = true;
  synth::channels[slot].adsr_activation_time = to_ms_since_boot(get_absolute_time());
  synth::channels[slot].trigger_attack();
  // printf("note on");
}
void set_note_off(int slot, int note, int velocity) {
  synth::channels[slot].gate = false;
  synth::channels[slot].trigger_release();
  // printf("note off");
}

void note_priority(int status, int note, int velocity) {
  // Last-note priority... add in lowest/highest
  switch (status)  { //check which type we received
    case 0x90:
      if (velocity>0)  {   //is velocity 0?  if so, we want it to be thought of as note off
        int8_t slot = -1; // means if no free voices are left, it will be -1 still

        for (int i = 0; i < MAX_VOICES; i++)  {
          if (synth::channels[i].note == note && synth::channels[i].is_active) { 
            slot = i;
            break;  // breaks for loop as a free slot has been found
          }
          if (!synth::channels[i].is_active) {
            slot = i;
            break;
          }
          
        }
        // should skip this is a free voice is found
        if (slot<0) {
          int8_t oldest_slot = -1;
          uint32_t longest_time = to_ms_since_boot(get_absolute_time()); // may not need the other two
          
          // oldest note used first
          uint32_t longest_released_time = longest_time; // for keeping track of the slot which has been held the longest
          uint32_t longest_active_time = longest_time; // for keeping track of the slot which has been held the longest
          for (int i = 0; i < MAX_VOICES; i++)  {
            if (!synth::channels[i].gate && (synth::channels[i].adsr_activation_time<longest_released_time)) {
              longest_released_time = synth::channels[i].adsr_activation_time;
              slot = i; // shouldn't be called unless theres one or clots notes in release, and then should give the oldest
            }
            //still active
            else if (synth::channels[i].adsr_activation_time<longest_active_time) {
              longest_active_time = synth::channels[i].adsr_activation_time;
              oldest_slot = i; // will give the oldest slot thats still being used
            }
          }

          // lastest note used first (keeps first held notes intact, and uses the more recent notes)
          // uint32_t longest_released_time = 0; // for keeping track of the slot which has been held the longest
          // uint32_t longest_active_time = 0; // for keeping track of the slot which has been held the longest
          // for (int i = 0; i < MAX_VOICES; i++)  {
          //   if (!synth::channels[i].gate && (synth::channels[i].adsr_activation_time>longest_released_time)) {
          //     longest_released_time = synth::channels[i].adsr_activation_time;
          //     slot = i; // shouldn't be called unless theres one or clots notes in release, and then should give the oldest
          //     printf("released slot %u \n", slot);
          //   }
          //   //still active
          //   else if (synth::channels[i].adsr_activation_time>longest_active_time) {
          //     longest_active_time = synth::channels[i].adsr_activation_time;
          //     oldest_slot = i; // will give the oldest slot thats still being used
          //     // printf("finding active slot %u \n", i);
          //   }
          // }


          // is there still no free slots?
          if (slot < 0) {
            slot = oldest_slot; // if theres a note thats been released already, use the oldest one, otherwise, use the longest held note
          }
        }

        // synth::channels[slot].off(); // clear it before setting it again (resets the ADSR, hopefully)

        if (NOTE_DEBUG) printf("Slot: %d Note: %d \n", slot, note);
        
        set_note_on(slot, note, velocity);
        
        break;  //only breaks if velocity > 0.  otherwise, the switch just rolls onto next case, note_off.
      }

    case 0x80:
      for (int voice = 0; voice < MAX_VOICES; voice++)  {
        if (synth::channels[voice].note == note)  { //check for a matching note
          set_note_off(voice, note, velocity);
          //no break here just in case there are somehow multiple of the same note stuck on
        }
      }
      break;
    default:
      break;
  }
}