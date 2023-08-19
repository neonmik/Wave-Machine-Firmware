#include "note_handling.h"

#include "../midi.h"

namespace NOTE_HANDLING {

  voice_data_t VOICES[POLYPHONY];

  // Synth Note Control
  void voice_on(int slot, int note, int velocity) {
    if (note) {
      VOICES[slot].on(note);

      filter_on();

      MIDI::sendNoteOn(VOICES[slot].note, velocity);
      QUEUE::trigger_send(slot, VOICES[slot].note, VOICES[slot].gate);
    } else {
      return;
    }
  }
  void voice_off(int slot, int note, int velocity) {

    VOICES[slot].off();

    filter_off();

    MIDI::sendNoteOff(VOICES[slot].note, velocity);
    QUEUE::trigger_send(slot, VOICES[slot].note, VOICES[slot].gate);
  }

  uint8_t voices_get (uint8_t slot) {
    if (VOICES[slot].gate) {
      return VOICES[slot].note;
    } else {
      return 0;
    }
  }

  void voices_stop() {
    // clears the notes from the synth, but not the array storing the info
    for (int i = 0; i < POLYPHONY; i++) {
      QUEUE::trigger_send(i, 0, false);
    }
    filter_off();
  }

  void voices_panic() {
    // clears the notes from the vocies, the synths array, and sends out midi note clearing... may need reworking for actual panic (all midi notes)
    for (int i = 0; i < POLYPHONY; i++) {
      voice_off(i, 0, 0);
      voices_dec();
    }
    filter_off();
  }

  void filter_on(void) {
    switch (_mode) {
      case Mode::MONO:
        if (!_filter_active && voices_active()) { 
          QUEUE::trigger_send(FILTER_VOICE, 0, true);
          _filter_active = true;
        }
        break;
      case Mode::PARA:
        if (voices_active()) { 
          QUEUE::trigger_send(FILTER_VOICE, 0, true);
          _filter_active = true;
        }
        break;
    }    
  }
  void filter_off(void) {
    // FOR MONO MODE
    if (_filter_active && !voices_active()) {
      QUEUE::trigger_send(FILTER_VOICE, 0, false);
      _filter_active = false;
    }
  }
  void filter_refresh (void) {
    // a little function for refreshing the filter on more notes with arp
    _filter_active = false;
  }

  // Priority Control
  void priority(int note, int velocity) {
    volatile int8_t voice = -1; // means if no free voices are left, it will be -1 still

    for (int i = 0; i < POLYPHONY; i++)  {
      // voice is being used, but by this note, so fire again
      if (VOICES[i].note == note && VOICES[i].gate) { 
        voice = i;
        break;  // breaks for-loop as a free slot has been found
      }

      // Voice is free
      if (!VOICES[i].active) {
        voice = i;
        voices_inc();
        break;
      }
    }

    // should skip this is a free voice is found
    if (voice < 0) {
      int8_t priority_voice = -1;
      volatile uint32_t time_now = to_ms_since_boot(get_absolute_time());
      
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
      if (voice > 0) {
        voices_inc();
      }
      // No slots in release? Use the next priority appropriate active voice
      if (voice < 0) {
        voice = priority_voice;
      }
    }
    if (_sustain) {
      // if a voice is allocated while sustain is pressed, but not released, the sustain should be reset until its released again, this should stop current notes being released on the pedal being released. 
      // I think this should apply to every voice? not just new voices... 
      VOICES[voice].sustained = false;
    }
    voice_on(voice, note, velocity);
  }

  void release(int note, int velocity) {
    if (!_sustain) {
      for (int8_t voice = 0; voice < POLYPHONY; voice++)  {
        if (VOICES[voice].note == note)  {
          if (VOICES[voice].gate) voices_dec();
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

  void update() {

    // Check the note-finished messages from Synth Core.
    check_release();

    if (_sustain_just_released) {
      for (int8_t voice = 0; voice < POLYPHONY; voice++) {
        if (VOICES[voice].sustained) {
          voices_dec(); // clears the filter control
          voice_off(voice, 0, 0); // clears the voice and sends note off midi messages
          VOICES[voice].sustained = false;
        }
      }
      _sustain_just_released = false;
    }

    // Update Arp notes if active.
    ARP::update();


  }
  void note_on (uint8_t note, uint8_t velocity) {
    if (!ARP::get_state()) priority(note, velocity); // synth voice allocation
    else {
      ARP::add_note(note);
    }
  }
  void note_off(uint8_t note, uint8_t velocity) {
    if (!ARP::get_state()) release(note, velocity); // synth voice allocation
    else {
      ARP::remove_note(note);
    }
  }
  void sustain_pedal(uint16_t status) {
    // bit shift to binary number (yes or no)
    bool temp = (status >> 9);

    if (_sustain != temp) {
      // if the input is new, then set it.
      _sustain = temp;
      if (ARP::get_state()) {
        // arp notes
        ARP::set_sustain(_sustain); // ???
      } else {
        if (!_sustain) _sustain_just_released = true;
      }
      // still not right - if sustain is released just before pressing new notes, it clears the notes just pressed... this needs to be handled before new notes are added...
    }
  }

  void        voices_inc (void) {
    ++_voices_active;
    if (_voices_active > POLYPHONY) {
        _voices_active = POLYPHONY;
    }
  }
  void        voices_dec (void) {
    --_voices_active;
    if (_voices_active < 0) {
        _voices_active = 0;
    }
  }
  void        voices_clr (void) {
    _voices_active = 0;
  }
  void        voices_set (uint8_t voices) {
    _voices_active = voices;
  }
  bool        voices_active (void) {
    return _voices_active;
  }
}