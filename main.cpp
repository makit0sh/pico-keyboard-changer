/* 
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 * Copyright (c) 2022 makit0sh
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board.h"
#include "tusb.h"

#include "usb_descriptors.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/bootrom.h"

#ifdef __cplusplus
 extern "C" {
#endif
#include "pio_usb.h"
#ifdef __cplusplus
}
#endif

#include "keycode_change.hpp"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

void led_blinking_task(void);

// gpio pin number for usb d plus connector
// d minus should be connected to PIO_USB_DP_PIN+1 gpio pin
#define PIO_USB_DP_PIN 16

static usb_device_t *usb_device = NULL;

void core1_main(void);
void usb_report_check(void);


/*------------- MAIN -------------*/
int main(void)
{
  board_init();
  tusb_init();

  // all USB task run in core1
  multicore_reset_core1();
  multicore_launch_core1(core1_main);

  while (1)
  {
    tud_task(); // tinyusb device task
    led_blinking_task();
    usb_report_check();
  }

  return 0;
}

//--------------------------------------------------------------------+
// host USB tasks
//--------------------------------------------------------------------+

void core1_main() {
  sleep_ms(10);

  // To run USB SOF interrupt in core1, create alarm pool in core1.
  static pio_usb_configuration_t config = PIO_USB_DEFAULT_CONFIG;
  config.pin_dp = PIO_USB_DP_PIN;
  config.alarm_pool = (void*)alarm_pool_create(2, 1);
  usb_device = pio_usb_host_init(&config);

  while (true) {
    pio_usb_host_task();
  }
}

void usb_report_check() {
  if (blink_interval_ms == BLINK_MOUNTED && usb_device != NULL) {
    for (int dev_idx = 0; dev_idx < PIO_USB_DEVICE_CNT; dev_idx++) {
      usb_device_t *device = &usb_device[dev_idx];
      if (!device->connected) {
        continue;
      }

      // Print received packet to EPs
      for (int ep_idx = 0; ep_idx < PIO_USB_DEV_EP_CNT; ep_idx++) {
        endpoint_t *ep = pio_usb_get_endpoint(device, ep_idx);

        if (ep == NULL) {
          break;
        }

        uint8_t temp[64];
        int len = pio_usb_get_in_data(ep, temp, sizeof(temp));

        if (len > 0) {
          if (len >= 8) { // 6-key rollover, TODO ignoring N-key
            uint8_t modifiers = temp[0];
            uint8_t reserved  = temp[1];
            if (reserved != 0) continue;
            uint8_t keycode[6] = { 0 };
            uint8_t changed_keycode[6] = { 0 };
            uint8_t changed_modifiers;
            for (int i = 0; i < 6; i++) {
              keycode[i] = temp[i+2];
            }
            change_keycode(keycode, modifiers, changed_keycode, changed_modifiers);

            tud_hid_keyboard_report(REPORT_ID_KEYBOARD, changed_modifiers, changed_keycode);
            // tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifiers, keycode);
          }
        }
      }
    }
  }
  return;
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when sent REPORT successfully to host
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint8_t len)
{
  (void) instance;
  (void) len;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;

  // if (report_type == HID_REPORT_TYPE_OUTPUT)
  // {
  //   // Set keyboard LED e.g Capslock, Numlock etc...
  //   if (report_id == REPORT_ID_KEYBOARD)
  //   {
  //     // bufsize should be (at least) 1
  //     if ( bufsize < 1 ) return;

  //     uint8_t const kbd_leds = buffer[0];

  //     if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
  //     {
  //       // Capslock On: disable blink, turn led on
  //       blink_interval_ms = 0;
  //       board_led_write(true);
  //     }else
  //     {
  //       // Caplocks Off: back to normal blink
  //       board_led_write(false);
  //       blink_interval_ms = BLINK_MOUNTED;
  //     }
  //   }
  // }
}

//--------------------------------------------------------------------+
// BLINKING TASK
//--------------------------------------------------------------------+
void led_blinking_task(void)
{
  static uint32_t start_ms = 0;
  static bool led_state = false;

  // blink is disabled
  if (!blink_interval_ms) return;

  // Blink every interval ms
  if ( board_millis() - start_ms < blink_interval_ms) return; // not enough time
  start_ms += blink_interval_ms;

  board_led_write(led_state);
  led_state = 1 - led_state; // toggle
}
