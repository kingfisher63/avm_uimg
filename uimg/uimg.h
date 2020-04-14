/*
 * Copyright (C) 2019 - Felix Schmidt (original Linux code)
 * Copyright (C) 2020 - Roger Hünen (windows port and enhancements)
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
#include <stdlib.h>

/*
 * uimg header
 *
 * Note: all data is stored in big endian byte order.
 */

#define	UIMG_MAGIC				0x56455233U	// VER3
#define	UIMG_NAME_LEN			20
#define	UIMG_MAX_PARTITIONS		20

typedef struct
{
	uint8_t			ud_reserved;	// Reserved (0)
	uint8_t			ud_year;		// Year - 1900 (0-255)
	uint8_t			ud_mon;			// Month (1-12)
	uint8_t			ud_mday;		// Day (1-31)
}
uimg_date_t;

typedef struct
{
	uint8_t			ut_reserved;	// Reserved (0)
	uint8_t			ut_hour;		// Hour (0-23)
	uint8_t			ut_min;			// Minute (0-59)
	uint8_t			ut_sec;			// Second (0-59)
}
uimg_time_t;

typedef struct
{
	uint32_t		uh_magic;
	char			uh_name[UIMG_NAME_LEN];
	uimg_date_t		uh_cdate;		// Creation date
	uimg_time_t		uh_ctime;		// Creation time (UTC)
	uint32_t		uh_version;		// Version (3)
	uint32_t		uh_size;		// File size (bytes)
	uint32_t		uh_num_part;	// Number of partitions (1-UIMG_MAX_PARTITIONS)
	uint32_t		uh_head_crc;	// CRC32 of head with uh_head_crc = 0
	uint32_t		uh_data_crc;	// CRC32 of all data partitions without head
	uint32_t		uh_part_crc[UIMG_MAX_PARTITIONS];
	uint32_t		uh_part_size[UIMG_MAX_PARTITIONS];
	uint8_t			uh_part_dev[UIMG_MAX_PARTITIONS];
	uint8_t			uh_part_data[UIMG_MAX_PARTITIONS];
}
uimg_head_t;

typedef struct
{
	uint8_t			udn_dev;
	const char*		udn_name;
}
uimg_dev_to_name_t;

extern uimg_dev_to_name_t	uimg_dev_to_name[];

extern errno_t	uimg_copy_part	( int in_fd, int out_fd, uint32_t size, uint32_t* uimg_crc_state, uint32_t* part_crc );
extern void		uimg_pack		( const char* fname, const char* prefix );
extern void		uimg_unpack		( const char* fname, const char* prefix, int unpack );
