/* Copyright 2021 @ Grayson Carr
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "rgb_matrix_user.h"
#include "keymap_user.h"

#ifdef CAPS_LOCK_INDICATOR_ENABLE
int16_t not_caps_indicators[DRIVER_LED_TOTAL];
#endif

#ifdef FN_LAYER_TRANSPARENT_KEYS_OFF
int16_t not_fn_indicators[DRIVER_LED_TOTAL];
#endif

keypos_t led_index_key_position[DRIVER_LED_TOTAL];

void keyboard_post_init_user(void) {
    rgb_matrix_disable();

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t led_index = g_led_config.matrix_co[row][col];
            if (led_index != NO_LED) {
                led_index_key_position[led_index] = (keypos_t){.row = row, .col = col};
            }
        }
    }

    //replace below func and storing that is required here with `default_layer_state_set_user()`
}

bool dip_switch_update_user(uint8_t index, bool active) { 
    uint8_t base_layer = (1UL << (active ? 2 : 0));
    store_all_non_indicators(base_layer);
    return true;
}

void store_all_non_indicators(uint8_t base_layer) {
#ifdef CAPS_LOCK_INDICATOR_ENABLE
    store_non_indicators(not_caps_indicators, base_layer, is_caps_lock_indicator);
#endif

#ifdef FN_LAYER_TRANSPARENT_KEYS_OFF
    store_non_indicators(not_fn_indicators, base_layer + 1, is_active);
#endif
}

void store_non_indicators(int16_t non_indicators[DRIVER_LED_TOTAL], uint8_t layer, bool (*is_indicator)(uint16_t)) {
    memset(non_indicators, -1, sizeof int16_t * DRIVER_LED_TOTAL);
    
    for (uint8_t i = 0U, uint8_t j = 0U; i < DRIVER_LED_TOTAL; i++) {
        uint16_t keycode = keymap_key_to_keycode(layer, led_index_key_position[i]);

        if (!(*is_indicator)(keycode)) {
            not_caps_indicators[j++] = i;
        }
    }
}

void rgb_matrix_indicators_user(void) {
    uint8_t current_layer = get_highest_layer(layer_state);
    switch (current_layer) {
#ifdef CAPS_LOCK_INDICATOR_ENABLE
        case MAC_BASE:
        case WIN_BASE:
            if (host_keyboard_led_state().caps_lock) {
                rgb_matrix_set_color_by_keycode(led_min, led_max, current_layer, is_not_caps_lock_indicator);
            }
            break;
#endif
#ifdef FN_LAYER_TRANSPARENT_KEYS_OFF
        case MAC_FN:
        case WIN_FN:
            rgb_matrix_set_color_by_keycode(led_min, led_max, current_layer, is_inactive);
            break;
#endif
    }
}

void rgb_matrix_set_color_by_keycode(uint8_t led_min, uint8_t led_max, uint8_t layer, bool (*is_keycode)(uint16_t)) {
    for (uint8_t i = led_min; i < led_max; i++) {
        uint16_t keycode = keymap_key_to_keycode(layer, led_index_key_position[i]);
        if ((*is_keycode)(keycode)) {
            rgb_matrix_set_color(i, RGB_OFF);
        }
    }
}

bool is_caps_lock_indicator(uint16_t keycode) {
#ifdef CAPS_LOCK_INDICATOR_LIGHT_ALPHAS
    return keycode == KC_CAPS || (keycode <= KC_Z && keycode >= KC_A);
#else
    return keycode == KC_CAPS;
#endif
}

bool is_active(uint16_t keycode) {
    return keycode != KC_TRNS && keycode != KC_NO;
}
