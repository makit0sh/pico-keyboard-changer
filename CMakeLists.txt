cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

include(pico_sdk_import.cmake)

project(pico-usb-keyboard-changer_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
pico_sdk_init()

set(PICO_PIO_USB_DIR ${CMAKE_CURRENT_LIST_DIR}/Pico-PIO-USB)

add_library(Pico-PIO-USB STATIC
    ${PICO_PIO_USB_DIR}/pio_usb.c
    ${PICO_PIO_USB_DIR}/usb_crc.c
)
pico_generate_pio_header(Pico-PIO-USB ${PICO_PIO_USB_DIR}/usb_tx.pio)
pico_generate_pio_header(Pico-PIO-USB ${PICO_PIO_USB_DIR}/usb_rx.pio)
target_link_libraries(Pico-PIO-USB PRIVATE
  pico_stdlib
  pico_multicore
  hardware_pio
  hardware_dma
)
target_include_directories(Pico-PIO-USB PRIVATE ${PICO_PIO_USB_DIR})

add_executable(usb_keyboard_changer)

target_sources(usb_keyboard_changer PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        )

# Make sure TinyUSB can find tusb_config.h
target_include_directories(usb_keyboard_changer PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${PICO_PIO_USB_DIR}
        )

# In addition to pico_stdlib required for common PicoSDK functionality, add dependency on tinyusb_device
# for TinyUSB device support and tinyusb_board for the additional board support library used by the example
target_link_libraries(usb_keyboard_changer PUBLIC
  pico_stdlib
  tinyusb_device tinyusb_board
  pico_multicore
  Pico-PIO-USB)

pico_add_extra_outputs(usb_keyboard_changer)

