/*
 * Copyright (c) 2023 e12.io
 * All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef H_E12_VARIANTS
#define H_E12_VARIANTS

#ifdef ARDUINO_SAMD_ZERO  //__SAMD21__
#warning "SAMD21 detected"
#define Serial SerialUSB
#define E12_INTR_PIN 3
#define WAKEUP_INTR_PIN 2
#define ONE_WIRE_BUS 4
#elif ARDUINO_RASPBERRY_PI_PICO
#define E12_INTR_PIN 2
#define WAKEUP_INTR_PIN 22
#define ONE_WIRE_BUS 15
#include "pico/stdlib.h"
#else
#endif

#endif
