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

#include <WinSock2.h>

void
uimg_unpack( const char* uimg_fname, const char* prefix, int unpack )
{
	int			uimg_fd;
	uimg_head_t	uimg_head;
	uint32_t	uimg_head_crc1;
	uint32_t	uimg_head_crc2;
	uint32_t	uimg_data_crc1;
	uint32_t	uimg_data_crc2;
	uint32_t	uimg_data_crc_state;
	uint32_t	uimg_magic;
	char		part_fname[_MAX_PATH];
	int			part_fd;
	uint32_t	part_size;
	uint32_t	part_crc1, part_crc2;

	if (_sopen_s(&uimg_fd, uimg_fname, _O_RDONLY|_O_BINARY, _SH_DENYWR, 0) != 0) {
		perror(uimg_fname);
		exit(1);
	}

	if (_read(uimg_fd, &uimg_head, sizeof(uimg_head)) != sizeof(uimg_head)) {
		fprintf(stderr, "%s: read header failed\n", uimg_fname);
		exit(1);
	}

	uimg_magic = ntohl(uimg_head.uh_magic);
	if (uimg_magic != UIMG_MAGIC) {
		fprintf(stderr, "Bad magic number (0x%08x)\n", uimg_magic);
		exit(1);
	}

	uimg_head_crc1 = ntohl(uimg_head.uh_head_crc);
	uimg_data_crc1 = ntohl(uimg_head.uh_data_crc);

	uimg_head.uh_head_crc = 0;
	uimg_head_crc2 = crc32(&uimg_head, sizeof(uimg_head));
	uimg_head.uh_head_crc = htonl(uimg_head_crc1);

	if (uimg_head_crc2 == uimg_head_crc1) {
		printf("Image header CRC OK (0x%08x)\n", uimg_head_crc2);
	} else {
		printf("Image header CRC ERROR (crc=0x%08x, expected=0x%08x)\n", uimg_head_crc2, uimg_head_crc1);
	}

	printf("Created: %04d/%02d/%02d %02d:%02d:%02d UTC\n",
		uimg_head.uh_cdate.ud_year + 1900,
		uimg_head.uh_cdate.ud_mon,
		uimg_head.uh_cdate.ud_mday,
		uimg_head.uh_ctime.ut_hour,
		uimg_head.uh_ctime.ut_min,
		uimg_head.uh_ctime.ut_sec
	);

	uimg_data_crc_state = crc32_init();
	for (int i = 0; i < UIMG_MAX_PARTITIONS; i++) {
		if (uimg_head.uh_part_size[i] == 0) {
			break;
		}

		memset(part_fname, 0, sizeof(part_fname));

		for (int j = 0; uimg_dev_to_name[j].udn_name != NULL; j++) {
			if (uimg_head.uh_part_dev[i] == uimg_dev_to_name[j].udn_dev) {
				sprintf_s(part_fname, sizeof(part_fname), "%s_%02d_%s.bin", prefix, uimg_dev_to_name[j].udn_dev, uimg_dev_to_name[j].udn_name);
				break;
			}
		}

		if (part_fname[0] == 0) {
			sprintf_s(part_fname, sizeof(part_fname), "%s_%02d.bin", prefix, uimg_head.uh_part_dev[i]);
		}

		part_size = ntohl(uimg_head.uh_part_size[i]);
		part_crc1 = ntohl(uimg_head.uh_part_crc[i] );

		printf(" %s size=%u crc=0x%08x", part_fname, part_size, part_crc1);

		if (unpack) {
			if (_sopen_s(&part_fd, part_fname, _O_WRONLY|_O_CREAT|_O_TRUNC|_O_BINARY, _SH_DENYRW, _S_IREAD|_S_IWRITE) != 0) {
				printf("\n");
				perror(part_fname);
				exit(1);
			}

			if (uimg_copy_part(uimg_fd, part_fd, part_size, &uimg_data_crc_state, &part_crc2) != 0) {
				printf(" COPY FAILED\n");
			} else
			if (part_crc2 == part_crc1) {
				printf(" OK\n");
			} else {
				printf(" CRC ERROR (0x%08x)\n", part_crc2);
			}

			_close(part_fd);
		} else {
			printf("\n");
		}
	}

	if (unpack) {
		uimg_data_crc2 = crc32_value(uimg_data_crc_state);

		if (uimg_data_crc2 == uimg_data_crc1) {
			printf("Image data CRC OK (0x%08x)\n", uimg_data_crc2);
		} else {
			printf("Image data CRC ERROR (crc=0x%08x, expected=0x%08x)\n", uimg_data_crc2, uimg_data_crc1);
		}
	}

	_close(uimg_fd);
}
