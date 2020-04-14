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

#include "crc32.h"
#include "uimg.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <WinSock2.h>

void
uimg_pack( const char* uimg_fname, const char* prefix )
{
	int				uimg_fd;
	uimg_head_t		uimg_head;
	uint32_t		uimg_data_crc_state;
	uint32_t		uimg_data_crc;
	uint32_t		uimg_size = sizeof(uimg_head);
	int				part_index = 0;
	char			part_fname[_MAX_PATH];
	struct _stat	part_stat;
	int				part_fd;
	uint32_t		part_crc;

	if (_sopen_s(&uimg_fd, uimg_fname, _O_WRONLY | _O_CREAT | _O_TRUNC | _O_BINARY, _SH_DENYRW, _S_IREAD | _S_IWRITE) != 0) {
		perror(uimg_fname);
		exit(1);
	}

	// Write empty header (to be updated later)
	memset(&uimg_head, 0, sizeof(uimg_head));
	if (_write(uimg_fd, &uimg_head, sizeof(uimg_head)) != sizeof(uimg_head)) {
		perror(uimg_fname);
		exit(1);
	}

	uimg_data_crc_state = crc32_init();
	for (int i = 0; i < 100 && part_index < UIMG_MAX_PARTITIONS; i++) {
		part_fname[0] = 0;

		// Try filename with partition name suffix
		for (int j = 0; uimg_dev_to_name[j].udn_name != NULL; j++) {
			if (i == uimg_dev_to_name[j].udn_dev) {
				sprintf_s(part_fname, sizeof(part_fname), "%s_%02d_%s.bin", prefix, i, uimg_dev_to_name[j].udn_name);
				if (_stat(part_fname, &part_stat) != 0) {
					part_fname[0] = 0;
				}
				break;
			}
		}

		// Try filename without partition name suffix
		if (part_fname[0] == 0) {
		sprintf_s(part_fname, sizeof(part_fname), "%s_%02d.bin", prefix, i);
			if (_stat(part_fname, &part_stat) != 0) {
				continue;
			}
		}

		// Process partition file

		printf("%s size=%u", part_fname, part_stat.st_size);

		if (_sopen_s(&part_fd, part_fname, _O_RDONLY|_O_BINARY, _SH_DENYWR, 0) != 0) {
			perror(part_fname);
			exit(1);
		}

		if (uimg_copy_part(part_fd, uimg_fd, part_stat.st_size, &uimg_data_crc_state, &part_crc) != 0) {
			printf(" COPY FAILED\n");
			perror(part_fname);
			exit(1);
		}

		printf(" crc=0x%08x\n", part_crc);

		uimg_head.uh_part_dev[part_index]  = i;
		uimg_head.uh_part_size[part_index] = htonl(part_stat.st_size);
		uimg_head.uh_part_crc[part_index]  = htonl(part_crc);
		part_index++;

		uimg_size += part_stat.st_size;

		_close(part_fd);

	}
	uimg_data_crc = crc32_value(uimg_data_crc_state);

	// Update header

	time_t		timestamp;
	struct tm	gmt;

	time(&timestamp);
	gmtime_s(&gmt, &timestamp);

	strcpy_s(uimg_head.uh_name, sizeof(uimg_head.uh_name), "Intel_Unified_Image");
	uimg_head.uh_magic         = htonl(UIMG_MAGIC);
	uimg_head.uh_cdate.ud_year = gmt.tm_year;
	uimg_head.uh_cdate.ud_mon  = gmt.tm_mon;
	uimg_head.uh_cdate.ud_mday = gmt.tm_mday;
	uimg_head.uh_ctime.ut_hour = gmt.tm_hour;
	uimg_head.uh_ctime.ut_min  = gmt.tm_min;
	uimg_head.uh_ctime.ut_sec  = gmt.tm_sec;
	uimg_head.uh_version       = htonl(3);
	uimg_head.uh_size          = htonl(uimg_size);
	uimg_head.uh_num_part      = htonl(part_index);
	uimg_head.uh_data_crc      = htonl(uimg_data_crc);
    uimg_head.uh_head_crc      = htonl(crc32(&uimg_head, sizeof(uimg_head)));

	// Write header

	_lseek(uimg_fd, 0, SEEK_SET);

	if (_write(uimg_fd, &uimg_head, sizeof(uimg_head)) != sizeof(uimg_head)) {
		perror(uimg_fname);
		exit(1);
	}

	_close(uimg_fd);
}
