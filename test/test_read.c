/* --------------------------------------------------------------------------

   MusicBrainz -- The Internet music metadatabase

   Copyright (C) 2013 Johannes Dewender

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

--------------------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>

#include <discid/discid.h>
#include "test.h"


int main(int argc, char *argv[]) {
	DiscId *d;
	int i, first, last;
	int subtest_passed;
	int offset, previous_offset;

	d = discid_new();

	announce("discid_read_sparse");
	if (!discid_read_sparse(d, NULL, 0)) {
		printf("SKIP\n");
		printf("\tNo disc found, SKIPPING!\n\n");
		discid_free(d);
		return 77; /* code for SKIP in autotools */
	}
	evaluate(1);


	announce("discid_get_id");
	evaluate(equal_int(strlen(discid_get_id(d)), 28));

	announce("discid_get_freedb_id");
	evaluate(equal_int(strlen(discid_get_freedb_id(d)), 8));

	announce("discid_get_submission_url");
	evaluate(strlen(discid_get_submission_url(d)) > 0);

	announce("discid_get_first_track_num");
	first = discid_get_first_track_num(d);
	evaluate(first > 0);
	announce("discid_get_last_track_num");
	last = discid_get_last_track_num(d);
	evaluate(last > 0);

	announce("discid_get_sectors");
	evaluate(equal_int(discid_get_sectors(d),
				discid_get_track_offset(d, last)
				+ discid_get_track_length(d, last)));

	announce("discid_get_track_offset sane");
	previous_offset = 0;
	subtest_passed = 0;
	for (i=first; i<=last; i++) {
		offset = discid_get_track_offset(d, i);
		if (offset <= discid_get_sectors(d)) {
			subtest_passed++;
		}
		if (previous_offset) {
			if (offset >= previous_offset) {
				subtest_passed++;
			}
		}
		previous_offset = offset;
	}
	evaluate(equal_int(subtest_passed, 2 * (last - first + 1) - 1));

	announce("discid_get_mcn empty");
	evaluate(strlen(discid_get_mcn(d)) == 0);

	announce("discid_get_track_isrc empty");
	subtest_passed = 0;
	for (i=first; i<=last; i++) {
		if (strlen(discid_get_track_isrc(d, i)) == 0) {
			subtest_passed++;
		}
	}
	evaluate(equal_int(subtest_passed, last - first + 1));


	discid_free(d);

	return !test_result();
}

/* EOF */
