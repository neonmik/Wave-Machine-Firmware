

int note2freq[] = {8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 19, 21, 22, 23, 24, 26, 28, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902, 8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544};

void set_note_on(int slot, int note, int velocity) {
  synth::voices[slot].note = note;
  synth::voices[slot].frequency = note2freq[synth::voices[slot].note];
  synth::voices[slot].trigger_attack();
  synth::voices[slot].activation_time = to_ms_since_boot(get_absolute_time());
  synth::voices[slot].is_active = true;
  
}
void set_note_off(int slot, int note, int velocity) {
  synth::voices[slot].trigger_release();
}
void note_priority(int status, int note, int velocity) {
  // Last-note priority... add in lowest/highest
  switch (status)  { //check which type we received
    case 0x90:
      if (velocity != 0)  {   //is velocity 0?  if so, we want it to be thought of as note off
        int slot = -1; // means if no free voices are left, it will be -1 still
        
        for (int i = 0; i < VOICE_COUNT; i++)  {
          if (synth::voices[i].note == 0) {
            slot = i;
            break;  //don't need to check any more if we've found one open slot
          }
        }
        if (slot < 0) { // no free voices left...
          int oldestSlot = -1;
          uint32_t max_time = 0xffffffff;
          uint32_t max_rel_time = 0xffffffff;
          for (int i=0; i < VOICE_COUNT; i++) {
           if (synth::voices[i].activation_time < max_rel_time &&  !synth::voices[i].is_active) {
             max_rel_time =  synth::voices[i].activation_time;
             slot = i; // oldest slot already in Release state
           }
           else if (synth::voices[i].activation_time < max_time) {
             max_time = synth::voices[i].activation_time;
             oldestSlot = i; // oldest note
           }
         }
         if (slot < 0)
            slot = oldestSlot;
        }

        synth::voices[slot].off();

        set_note_on(slot, note, velocity);
        break;  //only breaks if velocity > 0.  otherwise, the switch just rolls onto next case, noteOff.  clever.
      }

    case 0x80:
      for (int voice = 0; voice < VOICE_COUNT; voice++)  {
        if (synth::voices[voice].note == note)  { //check for a matching
          set_note_off(voice, note, velocity);
          //no break here just in case there are somehow multiple of the same note stuck on
        }
      }
      break;
    default:
      break;
  }
}
