# pico-usb-keyboard-changer
Hardware keyboard keycode changer with Raspberry Pi Pico

This is based on the hid_composite example from TinyUSB (https://github.com/hathach/tinyusb/tree/master/examples/device/hid_composite) 
and the example from Raspberry Pi Pico SDK (https://github.com/raspberrypi/pico-examples/tree/master/usb/device/dev_hid_composite).

## How to build

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```
Then use the generated uf2 file to flash to raspberry pi pico.

## Raspberry Pi Pico Wiring
Connect USB connector

| Raspberry Pi Pico PIN | USB Connector | 
| --------------------- | ------------- | 
| VBUS (Pin 40)         | VBUS          | 
| GND (Pin 38)          | GND           | 
| GP0 (Pin 0)           | D+            | 
| GP1 (Pin 1)           | D-            | 

GP16 pin can be reconfigured with macro PIO_USB_DP_PIN.

