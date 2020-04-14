/*
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

#include <stdio.h>
#include <string.h>

#define BADARG  (int)':'
#define	BADCH	(int)'?'
#define	EMSG	""

int		opterr = 1;	// Should error message be printed?
int		optind = 1;	// Index into parent argv vector
int		optopt;		// Character checked for validity
int		optreset;	// Reset getopt
char*	optarg;		// Argument associated with option

int
getopt( int nargc, char* const nargv[], const char* ostr )
{
	static char*	place = (char*)EMSG;	// Option letter processing
	const char*		oli;					// Option letter list index

	if (optreset || !*place) {				// Update scanning pointer
		optreset = 0;
		if (optind >= nargc || *(place = nargv[optind]) != '-') {
			place = (char*)EMSG;
			return (-1);
		}
		if (place[1] && *++place == '-') {	// Found "--"
			++optind;
			place = (char*)EMSG;
			return (-1);
		}
	}

	if ((optopt = (int)*place++) == (int)':' || !(oli = strchr(ostr, optopt))) {
		// If the user didn't specify '-' as an option, assume it means -1
		if (optopt == (int)'-') {
			return (-1);
		}
		if (!*place) {
			++optind;
		}
		if (opterr && *ostr != ':') {
			(void)printf("illegal option -- %c\n", optopt);
		}
		return (BADCH);
	}

	if (*++oli != ':') {	// Don't need argument
		optarg = NULL;
		if (!*place) {
			++optind;
		}
	} else {				// Need an argument
		if (*place) {		// No white space
			optarg = place;
		} else
		if (nargc <= ++optind) {	// No arg
			place = (char*)EMSG;
			if (*ostr == ':') {
				return (BADARG);
			}
			if (opterr) {
				(void)printf("option requires an argument -- %c\n", optopt);
			}
			return (BADCH);
		} else {			// White space
			optarg = nargv[optind];
		}
		place = (char*)EMSG;
		++optind;
	}

	return (optopt);	// Dump back option letter
}
