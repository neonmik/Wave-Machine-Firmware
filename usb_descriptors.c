/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "pico/unique_id.h"
#include "tusb.h"
#include "usb_descriptors.h"

// #include "test_mode.h"

// extern bool testMode;


/* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]       MIDI | HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n)  ( (CFG_TUD_##itf) << (n) )
#define USB_PID           (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
                           _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4) )
#define USB_VID   0x2E8A
#define USB_BCD   0x0200

// Configuration mode
// 0 : enumerated as CDC/MIDI. Board button is not pressed when enumerating
// 1 : enumerated as MSC. Board button is pressed when enumerating
bool USB_MODE = 0;

static uint32_t mode = 0;

//--------------------------------------------------------------------+
// Device Descriptors
//--------------------------------------------------------------------+

// Configuration for Just MIDI
tusb_desc_device_t const desc_device_0 =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = USB_BCD,
    .bDeviceClass       = 0x00,
    .bDeviceSubClass    = 0x00,
    .bDeviceProtocol    = 0x00,
    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor           = USB_VID,
    .idProduct          = USB_PID,
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};

// Configuration for Just MSC
tusb_desc_device_t const desc_device_1 =
{
    .bLength            = sizeof(tusb_desc_device_t),
    .bDescriptorType    = TUSB_DESC_DEVICE,
    .bcdUSB             = 0x0200,
    .bDeviceClass       = 0,
    .bDeviceSubClass    = 0,
    .bDeviceProtocol    = 0,

    .bMaxPacketSize0    = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor           = USB_VID,
    .idProduct          = USB_PID + 11, // should be different PID than desc0
    .bcdDevice          = 0x0100,

    .iManufacturer      = 0x01,
    .iProduct           = 0x02,
    .iSerialNumber      = 0x03,

    .bNumConfigurations = 0x01
};


// This code should allow us to switch configurations, but doesnt seem to.

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const * tud_descriptor_device_cb(void)
{
  mode = getUSBMode();
  
  return (uint8_t const*) (mode ? &desc_device_1 : &desc_device_0);
}


//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

// New setup for either MIDI or MSC... Kind want MIDI always on, and MSC to be able to turn on and off...

enum {
  ITF_0_NUM_MIDI = 0,
  ITF_0_NUM_MIDI_STREAMING,
  ITF_0_NUM_TOTAL,
};

enum {
  ITF_1_NUM_MSC = 0,
  ITF_1_NUM_TOTAL
};

#define CONFIG_0_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_MIDI_DESC_LEN) // add audio here in future possible update...
#define CONFIG_1_TOTAL_LEN    (TUD_CONFIG_DESC_LEN + TUD_MSC_DESC_LEN)

#define EPNUM_0_MIDI_OUT    0x01
#define EPNUM_0_MIDI_IN     0x81

#define EPNUM_1_MSC_OUT     0x02
#define EPNUM_1_MSC_IN      0x82


uint8_t const desc_configuration_0[] =
{
  // Config number, interface count, string index, total length, attribute, power in mA
  TUD_CONFIG_DESCRIPTOR(1, ITF_0_NUM_TOTAL, 0, CONFIG_0_TOTAL_LEN, 0x00, 100),

  // Interface number, string index, EP notification address and size, EP data address (out, in) and size.

  // Interface number, string index, EP Out & EP In address, EP size
  TUD_MIDI_DESCRIPTOR(ITF_0_NUM_MIDI, 0, EPNUM_0_MIDI_OUT, EPNUM_0_MIDI_IN, TUD_OPT_HIGH_SPEED ? 512 : 64),
};


uint8_t const desc_configuration_1[] =
{
  // Config number, interface count, string index, total length, attribute, power in mA
  TUD_CONFIG_DESCRIPTOR(1, ITF_1_NUM_TOTAL, 0, CONFIG_1_TOTAL_LEN, 0x00, 100),

  // Interface number, string index, EP Out & EP In address, EP size
  TUD_MSC_DESCRIPTOR(ITF_1_NUM_MSC, 0, EPNUM_1_MSC_OUT, EPNUM_1_MSC_IN, TUD_OPT_HIGH_SPEED ? 512 : 64),
};


// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
{
  (void) index; // for multiple configurations
  return mode ? desc_configuration_1 : desc_configuration_0;
}

// Old setup for both MIDI and MSC

// enum {
//   ITF_NUM_MIDI = 0,
//   ITF_NUM_MIDI_STREAMING,
//   ITF_NUM_MSC,
//   ITF_NUM_TOTAL
// };

// #define CONFIG_TOTAL_LEN  (TUD_CONFIG_DESC_LEN + TUD_MIDI_DESC_LEN + TUD_MSC_DESC_LEN)
// #define EPNUM_MIDI_OUT  0x00
// #define EPNUM_MIDI_IN   0x80
// #define EPNUM_MSC_OUT   0x01
// #define EPNUM_MSC_IN    0x81


// uint8_t const desc_fs_configuration[] =
// {
//   // Config number, interface count, string index, total length, attribute, power in mA
//   TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

//   // Interface number, string index, EP Out & EP In address, EP size
//   TUD_MIDI_DESCRIPTOR(ITF_NUM_MIDI, 0, EPNUM_MIDI_OUT, EPNUM_MIDI_IN, 64),
//    // Interface number, string index, EP Out & EP In address, EP size
//   TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 5, EPNUM_MSC_OUT, EPNUM_MSC_IN, 64),
// };

// #if TUD_OPT_HIGH_SPEED
// uint8_t const desc_hs_configuration[] =
// {
//   // Config number, interface count, string index, total length, attribute, power in mA
//   TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, 0, CONFIG_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),

//   // Interface number, string index, EP Out & EP In address, EP size
//   TUD_MIDI_DESCRIPTOR(ITF_NUM_MIDI, 0, EPNUM_MIDI_OUT, EPNUM_MIDI_IN, 512),
//   // Interface number, string index, EP Out & EP In address, EP size
//   TUD_MSC_DESCRIPTOR(ITF_NUM_MSC, 5, EPNUM_MSC_OUT, EPNUM_MSC_IN, 512),
// };
// #endif

// // Invoked when received GET CONFIGURATION DESCRIPTOR
// // Application return pointer to descriptor
// // Descriptor contents must exist long enough for transfer to complete
// uint8_t const * tud_descriptor_configuration_cb(uint8_t index)
// {
//   (void) index; // for multiple configurations

// #if TUD_OPT_HIGH_SPEED
//   // Although we are highspeed, host may be fullspeed.
//   return (tud_speed_get() == TUSB_SPEED_HIGH) ?  desc_hs_configuration : desc_fs_configuration;
// #else
//   return desc_fs_configuration;
// #endif
// }


//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

// buffer to hold flash ID
char serial[2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1];

// array of pointer to string descriptors
char const* string_desc_arr [] =
{
  (const char[]) { 0x09, 0x04 },            // 0: is supported language is English (0x0409)
  "NAMS Labs.",                             // 1: Manufacturer
  "Wave Machine Prototype",                 // 2: Product
  serial,                                   // 3: Serials, uses the flash ID
};

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
  (void) langid;

  uint8_t chr_count;

  if ( index == 0)
  {
    memcpy(&_desc_str[1], string_desc_arr[0], 2);
    chr_count = 1;
  }else
  {
    // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
    // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

    if (index == 3) pico_get_unique_board_id_string(serial, sizeof(serial));
    
    if ( !(index < sizeof(string_desc_arr)/sizeof(string_desc_arr[0])) ) return NULL;

    const char* str = string_desc_arr[index];

    // Cap at max char
    chr_count = strlen(str);
    if ( chr_count > 31 ) chr_count = 31;

    // Convert ASCII string into UTF-16
    for(uint8_t i=0; i<chr_count; i++)
    {
      _desc_str[1+i] = str[i];
    }
  }

  // first byte is length (including header), second byte is string type
  _desc_str[0] = (TUSB_DESC_STRING << 8 ) | (2*chr_count + 2);

  return _desc_str;
}