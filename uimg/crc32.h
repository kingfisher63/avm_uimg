/*
 * Copyright (C) 2019 - Felix Schmidt (original Linux code)
 * Copyright (C) 2020 - Roger H�nen (windows port and enhancements)
 *
 * This file is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include <stdint.h>

extern uint32_t	crc32			( const void* buf, size_t len );
extern uint32_t crc32_init		( void );
extern uint32_t	crc32_update	( const void* buf, size_t len, uint32_t crc_state );
extern uint32_t crc32_value		( uint32_t crc_state );
