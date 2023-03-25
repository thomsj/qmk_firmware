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

#pragma once

void store_all_non_indicators(uint8_t base_layer);
void store_non_indicators(int16_t non_indicators[], uint8_t layer, bool (*is_indicator)(uint16_t));
bool is_caps_lock_indicator(uint16_t keycode);
bool is_active(uint16_t keycode);
void rgb_matrix_set_color_by_keycode(int16_t non_indicators[]);
