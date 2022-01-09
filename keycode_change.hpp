/* 
 * The MIT License (MIT)
 *
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

#pragma once

#include <map>
#include "tusb.h"

void change_keycode(const uint8_t* keycode, const uint8_t modifiers, uint8_t* changed_keycode, uint8_t& changed_modifiers);

const uint8_t MODIFIER_LEFTCTRL   = TU_BIT(0); ///< Left Control
const uint8_t MODIFIER_LEFTSHIFT  = TU_BIT(1); ///< Left Shift
const uint8_t MODIFIER_LEFTALT    = TU_BIT(2); ///< Left Alt
const uint8_t MODIFIER_LEFTGUI    = TU_BIT(3); ///< Left Window
const uint8_t MODIFIER_RIGHTCTRL  = TU_BIT(4); ///< Right Control
const uint8_t MODIFIER_RIGHTSHIFT = TU_BIT(5); ///< Right Shift
const uint8_t MODIFIER_RIGHTALT   = TU_BIT(6); ///< Right Alt
const uint8_t MODIFIER_RIGHTGUI   = TU_BIT(7); ///< Right Window

struct Key {
    uint8_t code;
    bool shifted;

    Key(uint8_t code, bool shifted):code{code},shifted{shifted} {}

    bool operator < (Key const& rhs) const {
        return code < rhs.code;
    }
};

static std::map<uint8_t, Key> keycode_map_normal = {
    {HID_KEY_EQUAL,  Key(HID_KEY_MINUS, true)}, // =
    {HID_KEY_BRACKET_LEFT,  Key(HID_KEY_BRACKET_RIGHT, false)}, // {
    {HID_KEY_BRACKET_RIGHT,  Key(HID_KEY_EUROPE_1, false)}, // }
    {HID_KEY_APOSTROPHE, Key(HID_KEY_7, true)}, // '
    {HID_KEY_BACKSLASH, Key(HID_KEY_KANJI1, false)}, // \
    {HID_KEY_GRAVE, Key(HID_KEY_BRACKET_LEFT, true)} // `
};

static std::map<uint8_t, Key> keycode_map_shifted = {
    {HID_KEY_2,  Key(0x2F, false)}, // @
    {HID_KEY_6,  Key(0x2E, false)}, // ^
    {HID_KEY_7,  Key(HID_KEY_6, true)}, // &
    {HID_KEY_8,  Key(HID_KEY_APOSTROPHE, true)}, // *
    {HID_KEY_9,  Key(HID_KEY_8, true)}, // (
    {HID_KEY_0,  Key(HID_KEY_9, true)}, // )

    {HID_KEY_MINUS,  Key(0x87, true)}, // _
    {HID_KEY_EQUAL,  Key(HID_KEY_SEMICOLON, true)}, // +
    {HID_KEY_BRACKET_LEFT,  Key(HID_KEY_BRACKET_RIGHT, true)}, // [
    {HID_KEY_BRACKET_RIGHT,  Key(HID_KEY_EUROPE_1, true)}, // ]
    {HID_KEY_SEMICOLON, Key(HID_KEY_APOSTROPHE, false)}, // :
    {HID_KEY_APOSTROPHE, Key(HID_KEY_2, true)}, // "
    {HID_KEY_BACKSLASH, Key(HID_KEY_KANJI3, true)}, // |
    {HID_KEY_GRAVE, Key(HID_KEY_EQUAL, true)} // ~
};
