/*
 * Handles a pagination mecanism, each page can use the same knobs;
 * Digital switches select the current active page.
 *
 * This enables to "multiplex" the same knobs many times, safely thanks to the protection mecanism.
 *
 * After changing the active page, every knob is protected, not to force a jump in value.
 * On turning a knob the LED lights up when the knob's value matches the stored value, and then
 * the knob becomes active till next page switch.
 *
 * This mecanism is inspired and similar to that of the microKorg synth edit knobs.
 *
 */ 

//---------- USER INPUT AND PAGINATION -----------
#define abs(x) ({ __typeof__(x) _x = (x); _x >= 0 ? _x : -_x; })

#define MAX_PAGES         4 // the max number of pages available
#define MAX_KNOBS         4 // the max number of knobs available

#define PROTECTED        -1
#define ACTIVE            1

#define MAIN              0
#define ADSR              1
#define MOD               2
#define ARP               3

#define protection_value  5 // the amount of protection the knob gets before unlocking.

int page_values[MAX_PAGES][MAX_KNOBS]; // the permanent storage of every value for every page, used by the actual music code
int knob_values[MAX_KNOBS]; // last read knob values
int knob_states[MAX_KNOBS]; // knobs state (protected, enable...)

int value           = 0; // current (temporary) value just read
int current_page    = 0; // the current page id of values being edited
bool page_change    = false; // signals the page change
bool in_sync        = false; //temp variable to detect when the knob's value matches the stored value

void set_page (int page);

void default_pagination () {
  // ADSR default values
  page_values[ADSR][0]=0; //A
  page_values[ADSR][1]=0; // D
  page_values[ADSR][2]=1023; // S
  page_values[ADSR][3]=0; // R
}

void pagination_init() {
  // pinMode(aLED, OUTPUT);
  for(int i=0; i < MAX_KNOBS; i++){
    knob_values[i] = knobs[i];
    knob_states[i] = ACTIVE;
  }
  default_pagination();
}

// read knobs and digital switches and handle pagination
void pagination_get(){

  if(page_flag){
    page_change = true;

    // current_page = !current_page;
    // Or 
    int temp_pages = 0;
    current_page++;
    if (arp_flag) {
      temp_pages = MAX_PAGES; // sets the page loop to be it's full length
    }
    if (!arp_flag) temp_pages = (MAX_PAGES-1); // shrinks the loop so we dont end up with loads of pages we don't need when not using Arp.
    
    if (current_page >= temp_pages) current_page = 0;
    
    set_page(current_page);
    
    page_flag = 0;
  }

  // if page has changed then protect knobs
  if(page_change){
    page_change = false; // set the Page change back to false so it can be read again...

    // bitmask the knob leds off
    sr_shift_out((leds &= 0xF)); 

    for(int i=0; i < MAX_KNOBS; i++){ // loop through the array and set all the values to protected.
      knob_states[i] = PROTECTED;
    }
  }
  // read knobs values, show sync with the LED, enable knob when it matches the stored value
  for (int i = 0; i < MAX_KNOBS; i++){
    value = knobs[i];
    in_sync = abs(value - page_values[current_page][i]) < protection_value;

    // enable knob when it matches the stored value
    if (in_sync){
      knob_states[i] = ACTIVE;
    }
  
    // // if knob is moving, show if it's active or not
    // if (abs(value - knob_values[i]) > 5){
    //   // if knob is active, blink LED
    //   if(knob_states[i] == ACTIVE){
    //     // bitshift an ON to the current knob and output it to the ShiftReg
    //     sr_shift_out((leds |= 1 << (7 - i)));
    //   } else {
    //     // bitshift an OFF to the current knob and output it to the ShiftReg
    //     sr_shift_out((leds |= 0 << (7 - i)));
    //   }
    // }
    
    knob_values[i] = value;

    // if enabled then mirror the real time knob value
    if(knob_states[i] == ACTIVE){
      sr_shift_out((leds |= 1 << (7 - i)));
      page_values[current_page][i] = value;
    }
  }
}

void print_array(int *array, int len){
  for(int i = 0;i< len;i++){
    printf(" ");
    printf("%d", array[i]);
  }
}

void print_page(){
  printf("Page: %d   ", current_page);
  print_array(page_values[current_page], MAX_KNOBS);
  printf("\n");
}

