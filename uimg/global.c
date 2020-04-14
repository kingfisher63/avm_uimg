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

uimg_dev_to_name_t uimg_dev_to_name[] = {
	{  2, "ATOM_KERNEL" },
	{  3, "ATOM_ROOTFS" },
	{  8, "ARM_KERNEL"  },
	{  9, "ARM_ROOTFS"  },
	{ 10, "GWFS"        },
	{  0, NULL          }
};
