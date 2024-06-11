#include "note_handling.h"

#include "../midi.h"

namespace NOTE_HANDLING {

  VoiceData VOICES[POLYPHONY];

  // Synth Note Control
  void voiceOn(uint8_t slot, uint8_t note, uint8_t velocity) {
    if (!note) return;

    VOICES[slot].on(note, velocity);

    QUEUE::triggerSend(slot, VOICES[slot].note, VOICES[slot].gate);
  }
  void voiceOff(uint8_t slot, uint8_t note, uint8_t velocity) {

    VOICES[slot].off();

    QUEUE::triggerSend(slot, VOICES[slot].note, VOICES[slot].gate);
  }

  void voiceStop (uint8_t note){
    // stop the input note, whatever voice(s) it's on.
    for (int i = 0; i < POLYPHONY; i++) {
      if (VOICES[i].note == note) {
        voiceOff(i, note, 0);
        // no return/break here incase its on more than once
      }
    }
  }

  bool voicesCheckSustain (uint8_t slot) {
    if (VOICES[slot].sustained) { 
      return true;
    } else {
      return false;
    }
  }
  uint8_t voicesActiveNote (uint8_t slot) {
    // return the note stored in the slot if currently playing, don't return sustained notes as it messes with it.
    if (VOICES[slot].gate) { //}) && !VOICES[slot].sustained) {
      return VOICES[slot].note;
    } else {
      return 0;
    }
  }

  void voicesStop() {
    // stop all of notes from the synth, but not the array storing the info
    for (int i = 0; i < POLYPHONY; i++) {
      QUEUE::triggerSend(i, 0, false);
    }
  }


  void voicesPanic() {
    // clears the notes from the vocies, the synths array, and sends out midi note clearing... may need reworking for actual panic (all midi notes)
    for (int i = 0; i < POLYPHONY; i++) {
      voiceOff(i, 0, 0);
    }
  }

  // Priority Control
  void priority(uint8_t note, uint8_t velocity) {
    volatile int8_t voice = -1; // means if no free voices are left, it will be -1 still

    MIDI::sendNoteOn(note, velocity);

    for (int i = 0; i < POLYPHONY; i++)  {
      // voice is being used, but by the same note, so fire again
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

      volatile uint32_t time_now = sampleClock;
      
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
              priority_voice = i; // will give the newest slot thats still being used
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
    if (sustainPedal) {
      // if a voice is allocated while sustain is pressed, but not released, the sustain should be reset until its released again, this should stop current notes being released on the pedal being released. 
      // I think this should apply to every voice? not just new voices... 
      VOICES[voice].sustained = false;
    }
    voiceOn(voice, note, velocity);
  }

  void release(uint8_t note, uint8_t velocity) {
    if (!sustainPedal) {
      for (int8_t voice = 0; voice < POLYPHONY; voice++)  {
        if (VOICES[voice].note == note)  {
          MIDI::sendNoteOff(note, MIDI_DEFAULT_NOTE_OFF_VEL);
          voiceOff(voice, note, velocity);
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

  void checkReleaseMessages () {
    uint8_t queueLength = QUEUE::releaseCheckQueue();
    if (queueLength) {
      for (int i = 0; i < queueLength; i++) {
        // receives the slot number thats released from the queue, and then clears the slot on this core.
        uint8_t slot = QUEUE::releaseReceive();
        VOICES[slot].clear();
      }
    }
  }

  void update() {

    // Check the note-finished messages from Synth Core.
    checkReleaseMessages();

    if (isSustainJustReleased) {
      for (int8_t voice = 0; voice < POLYPHONY; voice++) {
        if (VOICES[voice].sustained) {
          release(VOICES[voice].note, DEFAULT_OFF_VEL);
        }
      }
      isSustainJustReleased = false;
    }

    // update Arp notes if active.
    ARP::update();
  }
  
  void noteOn (uint8_t note, uint8_t velocity) {
    // send midi notes out here - unless arp is active, in which case let it do it
    // also need to provide a way to stop MIDI notes being sent straight out...

    // MIDI::sendNoteOn(note, velocity);
    
    // set note as active in the grat big array
    noteState[note].note = 1;

    // do as before, but without MIDI things
    if (!ARP::getState()) priority(note, velocity); // synth voice allocation
    else {
      ARP::addNote(note);
    }
  }
  void noteOff(uint8_t note, uint8_t velocity) {
    // send midi notes out here - unless arp is active, in which case let it do it
    // also need to provide a way to stop MIDI notes being sent straight out...

    // MIDI::sendNoteOff(note, velocity);

    // set note as inactive in the great big array
    if (!sustainPedal) noteState[note].note = 0;

    // do as before, but without MIDI things
    if (!ARP::getState()) release(note, velocity); // synth voice allocation
    else {
      ARP::removeNote(note);
    }
  }
  void setSustainPedal(uint16_t status) {
    
    // bit shift to binary number (yes or no)
    bool temp = (status >> 9);

    if (sustainPedal != temp) {
      // if the input is new, then set it.
      sustainPedal = temp;

      // Code for Note Priority sustain
      if (!sustainPedal) isSustainJustReleased = true;

      // Code for Arp Latch:
      if (ARP::getState()) {
        // this code is acts like a momentary switch, only latching when held
        // ARP::toggleSustain(); 
        
        // this code toggles the Latch per press.
        if (sustainPedal) {
          ARP::toggleSustain(); 
        }
      }

    }
  }
  bool getSustainPedal (void) {
    return sustainPedal;
  }
}