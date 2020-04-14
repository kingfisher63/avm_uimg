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

#include "getopt.h"
#include "uimg.h"

#include <stdio.h>
#include <string.h>

typedef enum
{
	none,
	list,
	unpack,
	pack
}
mode_t;

static const char* help =
	"\n"
	" uimg -h|-v\n"
	" uimg -l|-p|-u [-n <name>] uimg-file\n"
	"\n"
	"   -h   print this message\n"
	"\n"
	"   -v   print program version\n"
	"\n"
	"   -l   list partitions\n"
	"\n"
	"   -p   pack partition files with -n prefix into uimg-file\n"
	"        All file names matching the above file format are added as partitions.\n"
	"        The file name can be with content _suffix (default) or without.\n"
	"\n"
	"   -u   unpack partitions and write .bin files named name_nn[_suffix].bin\n"
	"        - name   = given name or uimg-file (including path, without extension)\n"
	"        - nn     = logical partition number.\n"
	"        - suffix = name of partition content (if known by the tool)\n"
	"\n"
	"   -n   name prefix for output files (default: input file name without suffix)\n"
	"\n";

int
main( int argc, char** argv )
{
	char	prefix_buf[_MAX_FNAME];
	char*	prefix = NULL;
	char*	fname  = NULL;
	mode_t	mode   = none;
	int		i, c;

	while ((c = getopt(argc, argv, "lpun:hv")) != -1) {
		switch (c) {
			case 'h':
				printf(help);
				printf(" Known content names:\n\n");
				for (i = 0; uimg_dev_to_name[i].udn_name != NULL; i++) {
					printf("  %02d = %s\n", uimg_dev_to_name[i].udn_dev, uimg_dev_to_name[i].udn_name);
				}
				printf("\n");
				exit(0);

			case 'l':
				mode = list;
				break;

			case 'p':
				mode = pack;
				break;

			case 'u':
				mode = unpack;
				break;

			case 'n':
				prefix = _strdup(optarg);
				break;

			case 'v':
				printf("uimg v1.0\n\n");
				exit(0);

			default:
				exit(1);
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "uimg-file not specified (use 'uimg -h' to print help)\n");
		exit(1);
	}
	fname = argv[optind];

	if (!prefix) {
		if (_splitpath_s(fname, NULL, 0, NULL, 0, prefix_buf, sizeof(prefix_buf), NULL, 0) != 0) {
			perror(fname);
			exit(1);
		}
		prefix = prefix_buf;
	}

	switch (mode) {
		case list:
			uimg_unpack(fname, prefix, 0);
			break;

		case unpack:
			uimg_unpack(fname, prefix, 1);
			break;

		case pack:
			uimg_pack(fname, prefix);
			break;

		default:
			fprintf(stderr, "action not specified (use 'uimg -h' to print help)\n");
			exit(1);
	}

	return 0;
}
