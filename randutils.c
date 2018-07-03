/* randutils.c
 *
 * Copyright (C) 2018 Jeff Spaulding <sarnet@gmail.com>
 *
 * This is the ISC License.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "randutils.h"

#include <stdlib.h>
#include <stdio.h>

int
srand_from_file(char *filename)
{
	unsigned int seed;
	size_t items_read;
	FILE *fp;

	if (! (fp = fopen(filename, "r"))) {
		return 0;
	}

	items_read = fread(&seed, sizeof(unsigned int), 1, fp);

	fclose(fp);

	if (items_read == 1) {
		srand(seed);
		return 1;
	} else {
		return 0;
	}
}

char
rand_ascii()
{
	char c = 32;
	int x = 127;
	int tmp = 0;

	while (x > 94) {
		x = 0;
		tmp = rand();

		/* To prevent issues with low randomness in low-order bits,
		 * we xor each byte returned from rand() into x. */
		for (unsigned int i = 0; i < sizeof(int); i++) {
			x ^= tmp & 0xFF;
			tmp = tmp >> 8;
		}

		/* Chop off high order bit */
		x &= 0x7F;
	}

	c += x;

	return c;
}
