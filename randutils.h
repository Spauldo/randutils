/* randutils.h
 *
 * Quick and dirty random number related functions
 *
 * Anyone using this for crypto or anything important gets what they deserve.
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

#ifndef _RANDUTILS_H
#define _RANDUTILS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Reads the first (sizeof(int)) bytes from filename, casts it as an int, then
 * seeds using srand.  /dev/urandom is a good choice on Linux, with fallback to
 * /dev/random (which may block on some platforms).
 * Returns true if successful, false in case of failure. */
int srand_from_file(char *filename);

/* Returns a random printable ASCII character.
 * Uses rand(). */
char rand_ascii();


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RANDUTILS_H */
