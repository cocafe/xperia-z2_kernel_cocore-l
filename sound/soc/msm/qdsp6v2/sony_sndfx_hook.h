/* Copyright (c) 2016, cocafe@xda-developers.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

extern uint16_t sndfx_enable_hook;
extern uint16_t sndfx_enable;

extern uint16_t clearstereo_hook;
extern int32_t clearstereo_coef;

extern uint16_t cleareq_hook;

extern uint16_t clearvpt_hook;
extern uint16_t clearvpt_mode;

int16_t sndfx_hook_get_eqband(int index);
