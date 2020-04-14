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

#include "uimg.h"
#include "crc32.h"

#include <errno.h>
#include <io.h>

#define BUFFER_SIZE	(256*1024)

static char		buffer[BUFFER_SIZE];

errno_t uimg_copy_part( int in_fd, int out_fd, uint32_t size, uint32_t* uimg_crc_state, uint32_t* part_crc )
{
	uint32_t	block_size;
	int			nbytes;
	uint32_t	part_crc_state;

	part_crc_state = crc32_init();
	while (size > 0) {
		block_size = size < BUFFER_SIZE ? size : BUFFER_SIZE;

		nbytes = _read(in_fd, buffer, block_size);
		if (nbytes < 0) {
			return errno;
		}
		if (nbytes != block_size) {
			errno = EIO;
			return errno;
		}

		if (_write(out_fd, buffer, nbytes) != nbytes) {
			errno = EIO;
			return errno;
		}

	   *uimg_crc_state = crc32_update(buffer, nbytes, *uimg_crc_state);
		part_crc_state = crc32_update(buffer, nbytes,  part_crc_state);

		size -= block_size;
	}
   *part_crc = crc32_value(part_crc_state);

	return 0;
}
