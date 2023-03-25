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
}

layer_state_t default_layer_state_set_user(layer_state_t state) {
    uint8_t layer = IS_LAYER_ON_STATE(state, MAC_BASE)
        ? MAC_BASE
        : WIN_BASE;

    store_all_non_indicators(layer);
    return state;
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
            non_indicators[j++] = i;
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

void rgb_matrix_indicators_user(void) {
    uint8_t current_layer = get_highest_layer(layer_state);
    switch (current_layer) {
#ifdef CAPS_LOCK_INDICATOR_ENABLE
        case MAC_BASE:
        case WIN_BASE:
            // Don't turn off any keys if the user has manually turned on RGB
            if (host_keyboard_led_state().caps_lock) {
                rgb_matrix_set_color_by_keycode(not_caps_indicators);
            }
            break;
#endif
#ifdef FN_LAYER_TRANSPARENT_KEYS_OFF
        case MAC_FN:
        case WIN_FN:
            rgb_matrix_set_color_by_keycode(not_fn_indicators);
            break;
#endif
    }
}

void rgb_matrix_set_color_by_keycode(int16_t non_indicators[DRIVER_LED_TOTAL]) {
    for (uint8_t i = 0; i < DRIVER_LED_TOTAL; i++) {
        int16_t led_index = non_indicators[i];

        if (led_index < 0) {
            break;
        }

        rgb_matrix_set_color(i, RGB_OFF);
    }
}
