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

#include "keycode_change.hpp"

uint8_t change_modifiers(const uint8_t modifiers) {
    uint8_t changed_modifiers = modifiers;
    for (auto mod_pair : modifier_map) {
        if ((modifiers&mod_pair.first)!=0) {
            changed_modifiers &= ~mod_pair.first; 
            changed_modifiers |= mod_pair.second;
        }
    }
    return changed_modifiers;
}

void change_keycode(const uint8_t* keycode, const uint8_t modifiers, uint8_t* changed_keycode, uint8_t& changed_modifiers) {
    bool is_shifted = ((modifiers&(MODIFIER_LEFTSHIFT|MODIFIER_RIGHTSHIFT))!=0) ;

    std::map<uint8_t, Key>& keymap = is_shifted? keycode_map_shifted : keycode_map_normal;

    bool is_changed_modifier_shifted = is_shifted;
    uint8_t key_idx = 0;
    for (int i=0; i<6; i++) {
        if (keycode[i] == 0) continue;

        auto it = keymap.find(keycode[i]);
        if (it != keymap.end()) {
            if (i==0) { // use the first pressed key modifier
                is_changed_modifier_shifted = it->second.shifted;
            } else {
                if (it->second.shifted != is_changed_modifier_shifted) {
                    continue; // changed modifiers clashed, ignoring
                }
            }
            changed_keycode[key_idx] = it->second.code;
            key_idx++;
        } else {
            if (i==0) { // use the first pressed key modifier
                is_changed_modifier_shifted = is_shifted;
            } else {
                if (is_shifted != is_changed_modifier_shifted) {
                    continue; // changed modifiers clashed, ignoring
                }
            }
            changed_keycode[key_idx] = keycode[i];
            key_idx++;
        } 
    }

    changed_modifiers = modifiers;
    if (is_changed_modifier_shifted) {
        changed_modifiers |= MODIFIER_LEFTSHIFT;
    } else {
        changed_modifiers &= ~MODIFIER_LEFTSHIFT;
        changed_modifiers &= ~MODIFIER_RIGHTSHIFT;
    }
}
