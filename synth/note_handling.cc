#include "note_handling.h"

#include "../midi.h"

namespace NOTE_HANDLING {

  VoiceData VOICES[POLYPHONY];

  // Synth Note Control
  void voice_on(uint8_t slot, uint8_t note, uint8_t velocity) {
    if (!note) return;

    VOICES[slot].on(note, velocity);

    QUEUE::trigger_send(slot, VOICES[slot].note, VOICES[slot].gate);
  }
  void voice_off(uint8_t slot, uint8_t note, uint8_t velocity) {

    VOICES[slot].off();

    QUEUE::trigger_send(slot, VOICES[slot].note, VOICES[slot].gate);
  }

  bool voices_check (uint8_t slot) {
    if (VOICES[slot].sustained) { 
      return true;
    } else {
      return false;
    }
  }
  uint8_t voices_get (uint8_t slot) {
    // return the note stored in the slot if currently playing, don't return sustained notes as it messes with it.
    if (VOICES[slot].gate) { //}) && !VOICES[slot].sustained) {
      return VOICES[slot].note;
    } else {
      return 0;
    }
  }

  void voice_stop (uint8_t note){
    // stop the input note, whatever voice(s) it's on.
    for (int i = 0; i < POLYPHONY; i++) {
      if (VOICES[i].note == note) {
        voice_off(i, note, 0);
        // no return/break here incase its on more than once
      }
    }
  }

  void voices_stop() {
    // stop all of notes from the synth, but not the array storing the info
    for (int i = 0; i < POLYPHONY; i++) {
      QUEUE::trigger_send(i, 0, false);
    }
  }

  void voices_stop_all (void) {
    voices_stop();
    // voices_clr();
  }

  void voices_panic() {
    // clears the notes from the vocies, the synths array, and sends out midi note clearing... may need reworking for actual panic (all midi notes)
    for (int i = 0; i < POLYPHONY; i++) {
      voice_off(i, 0, 0);
      // voices_dec();
    }
    // filter_off();
  }

  // void filter_on(void) {
  //   switch (_mode) {
  //     case Mode::MONO:
  //       if (!_filter_active && voices_active()) { 
  //         QUEUE::trigger_send(FILTER_VOICE, 0, true);
  //         _filter_active = true;
  //       }
  //       break;
  //     case Mode::PARA:
  //       if (voices_active()) { 
  //         QUEUE::trigger_send(FILTER_VOICE, 0, true);
  //         _filter_active = true;
  //       }
  //       break;
  //   }    
  // }
  // void filter_off(void) {
  //   if (_filter_active && !voices_active()) {
  //     QUEUE::trigger_send(FILTER_VOICE, 0, false);
  //     _filter_active = false;
  //   }
  // }
  // void filter_refresh (void) {
  //   _filter_active = false;
  // }

  // Priority Control
  void priority(uint8_t note, uint8_t velocity) {
    volatile int8_t voice = -1; // means if no free voices are left, it will be -1 still

    MIDI::sendNoteOn(note, velocity);

    for (int i = 0; i < POLYPHONY; i++)  {
      // voice is being used, but by this note, so fire again
      if (VOICES[i].note == note && VOICES[i].gate) { 
        voice = i;
        break;  // breaks for-loop as a free slot has been found
      }

      // Voice is free
      if (!VOICES[i].active) {
        voice = i;
        break;
      }
    }

    // should skip this is a free voice is found
    if (voice < 0) {
      int8_t priority_voice = -1;

      volatile uint32_t time_now = sample_clock;
      
      switch (_priority) {
        // When there are no free voices on the synth, this code uses various priority types to assign the new note to a voice.
        case Priority::LAST: {
          // This mode assigns the new note to the voice with the oldest persisting voice (The Latest notes played have prioirty)
          uint32_t longest_released_time = time_now;
          uint32_t longest_active_time = time_now;
          for (int i = 0; i < POLYPHONY; i++)  {
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
          for (int i = 0; i < POLYPHONY; i++)  {
            // CHANGE!! from active to Gate?
            if (!VOICES[i].gate && (VOICES[i].activation_time>shortest_released_time)) { // released notes
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

        // These modes need checking, and I believe rewritting... probably can remove the active checker.
        // They should only be none active if they make it here, does it need to be gate? I don't think so.
        case Priority::LOWEST: {
            // This mode assigns the new note only if the new note is lower than one of the persisting notes (the lowest notes have priority)
            uint8_t lowest_note = 127; // Initialize to the highest possible MIDI note value
            for (int i = 0; i < POLYPHONY; i++) {
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
            for (int i = 0; i < POLYPHONY; i++) {
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
        // if using a priority voice, make sure the last note has sent a midi note off before using the voice?
        // for now do this, but could maybe do with a better implementation - mainly when using as a controler keyboard.
        // MAYBE just move MIDI::sendNoteOff outside of the check loop, so that its always called when it actually happens not at voice control stage.
        MIDI::sendNoteOff(VOICES[voice].note, MIDI_DEFAULT_NOTE_OFF_VEL);
      }
    }
    if (_sustain) {
      // if a voice is allocated while sustain is pressed, but not released, the sustain should be reset until its released again, this should stop current notes being released on the pedal being released. 
      // I think this should apply to every voice? not just new voices... 
      VOICES[voice].sustained = false;
    }
    voice_on(voice, note, velocity);
  }

  void release(uint8_t note, uint8_t velocity) {
    if (!_sustain) {
      for (int8_t voice = 0; voice < POLYPHONY; voice++)  {
        if (VOICES[voice].note == note)  {
          MIDI::sendNoteOff(note, MIDI_DEFAULT_NOTE_OFF_VEL);
          voice_off(voice, note, velocity);
        }
      }
    } else {
      for (int8_t voice = 0; voice < POLYPHONY; voice++)  {
        if (VOICES[voice].note == note) {
          VOICES[voice].sustained = true;
        }
      }
    }
  }

  void check_release () {
    uint8_t queue_level = QUEUE::release_check_queue();
    if (queue_level) {
      for (int i = 0; i < queue_level; i++) {
        // receives the slot number thats released from the queue, and then clears the slot on this core.
        uint8_t slot = QUEUE::release_receive();
        VOICES[slot].clear();
      }
    }
  }

  void Update() {

    // Check the note-finished messages from Synth Core.
    check_release();

    if (_sustain_just_released) {
      for (int8_t voice = 0; voice < POLYPHONY; voice++) {
        if (VOICES[voice].sustained) {
          voice_off(voice, VOICES[voice].note, 0); // clears the voice
          MIDI::sendNoteOff(VOICES[voice].note, MIDI_DEFAULT_NOTE_OFF_VEL); // sends midi note off
          VOICES[voice].sustained = false;
        }
      }
      _sustain_just_released = false;
    }

    // Update Arp notes if active.
    ARP::Update();


  }
  void note_on (uint8_t note, uint8_t velocity) {
    if (!ARP::get_state()) priority(note, velocity); // synth voice allocation
    else {
      ARP::addNote(note);
    }
  }
  void note_off(uint8_t note, uint8_t velocity) {
    if (!ARP::get_state()) release(note, velocity); // synth voice allocation
    else {
      ARP::removeNote(note);
    }
  }
  void sustain_pedal(uint16_t status) {
    
    // bit shift to binary number (yes or no)
    bool temp = (status >> 9);

    if (_sustain != temp) {
      // if the input is new, then set it.
      _sustain = temp;

      // Code for Note Priority sustain
      if (!_sustain) _sustain_just_released = true;

      // Code for Arp Latch:
      if (ARP::get_state()) {
        // this code is acts like a momentary switch, only latching when held
        // ARP::toggleSustain(); 
        
        // this code toggles the Latch per press.
        if (_sustain) {
          ARP::toggleSustain(); 
        }
      }

    }
  }
  bool getSustain (void) {
    return _sustain;
  }
}