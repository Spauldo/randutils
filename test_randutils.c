/* test_randutils.c
 *
 * Utility for testing librandutils
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

#define _POSIX_C_SOURCE 200809L

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <math.h>
#include <sys/types.h>

#include "randutils.h"

/* Create a temporary file and return a file descriptor to it.
 * Takes a pointer to char pointer as an argument.  It will allocate a
 * string and place it in the char pointer.  Be sure to free() this string
 * after use.
 * On failure, it frees the string and returns -1. */
int gen_tmp_file(char **fn_template);

/* Test the srand_from_file() function.  Creates two files with a different
 * number in each one, seeds from the first, generates a few random numbers,
 * seeds from the second, generates a few random numbers, then reseeds
 * from the first again and checks that the numbers match the first run but
 * not the second.  Has a very, very small chance of failing if the two
 * sequences are identical, but this is unlikely to ever happen. */
int test_srand_from_file();

/* Test the rand_ascii function.  Really just generates num_chars random
 * characters and prints them to stdout. */
void test_rand_ascii(int num_chars);

/* Print program usage to stderr */
void print_usage();

/* Global variable containing the program name.  Actually points to argc[0]
 * on main()'s stack.  See the relevant part of main() for my reasoning. */
char *progname;

int
main(int argc, char **argv)
{
	long num_chars = 50;
	int c;
	int cflag, sflag;

	/* Normally, setting a global to point to a stack variable is a really
	 * bad idea, but in this case it's pointing to a stack variable in
	 * main() which won't go away until the end of the program. */
	progname = argv[0];

	cflag = sflag = 0;

	while ((c = getopt(argc, argv, "hsc:")) != -1) {
		switch(c) {
		case 'c':
			num_chars = strtol(optarg, NULL, 10);
			cflag = 1;
			break;
		case 's':
			sflag = 1;
			break;
		default:
			print_usage();
			return EXIT_FAILURE;
		}
	}

	if (cflag && num_chars < 1) {
		fprintf(stderr, "Invalid argument to -c.");
		print_usage();
		return EXIT_FAILURE;
	}

	if (cflag && sflag) {
		fprintf(stderr, "Flags -s and -c are mutually exclusive.\n");
		print_usage();
		return EXIT_FAILURE;
	}

	if (! (cflag || sflag)) {
		fprintf(stderr, "Either flag -c or -s must be specified.\n");
		print_usage();
		return EXIT_FAILURE;
	}

	if (cflag)
		test_rand_ascii(num_chars);
	else if (sflag) {
		if (test_srand_from_file())
			printf("Seed test succeeded.\n");
		else {
			printf("Seed test failed.\n");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

/* Create a temporary file and return a file descriptor to it.
 * Takes a pointer to char pointer as an argument.  It will allocate a
 * string and place it in the char pointer.  Be sure to free() this string
 * after use.
 * On failure, it frees the string and returns -1. */
int
gen_tmp_file(char **fn_template)
{
	int pid = 0;            /* PID of this program */
	int fd = -1;            /* File descriptor */
	int fn_len = 0;         /* Length of the temporary filename */
	int bytes_printed = 0;  /* Length returned by snprintf() */

	pid += getpid();

	/* Compute the length of the filename.
	 * We need to add 1 to the result of log10(), 1 for the null byte,
	 * then 1 for each X we'll be using. */
	fn_len = trunc(log10(pid)) + strlen(progname) + 8;

	*fn_template = (char*) malloc(fn_len * sizeof(char));

	bytes_printed = snprintf(*fn_template, fn_len, "%s%dXXXXXX",
				 progname, pid);

	if (bytes_printed + 1 > fn_len) {
		free(*fn_template);
		return -1;
	}

	fd = mkstemp(*fn_template);

	if (fd == -1) {
		warn("gen_temp_file failed:");
		free(*fn_template);
	}

	return fd;
}

int
test_srand_from_file()
{
	FILE *f1, *f2;               /* File streams for temp files */
	int fd1, fd2;                /* File descriptors for temp files */
	char *filename1, *filename2; /* Filenames for temp files */
	int items_written = 0;       /* Number of values written to file */

	unsigned int rand_num1 = 1;  /* First seed */
	unsigned int rand_num2 = 2;  /* Second seed */

	const int num_tests = 100;

	int results[num_tests];      /* Results from calls to rand() */
	int num_identical = 0;       /* Number of tests that are identical */

	int retval = 0;              /* Value to return */

	/* OH NOES!!!  THERE'S A GOTO IN HERE!
	 * After the initial two calls to gen_temp_file(), any failures get
	 * sent to the cleanup: label. */

	/* Create two files for srand_from_file() to read from */

	fd1 = gen_tmp_file(&filename1);
	if (fd1 == -1) {
		return 0;
	}

	fd2 = gen_tmp_file(&filename2);
	if (fd2 == -1) {
		warn("gen_temp_file failed");
		free(filename1);
		return 0;
	}

	f1 = fdopen(fd1, "r+b");
	f2 = fdopen(fd2, "r+b");

	items_written  = fwrite(&rand_num1, sizeof(unsigned int), 1, f1);
	items_written += fwrite(&rand_num2, sizeof(unsigned int), 1, f2);

	if (items_written != 2) {
		warn("Wrote %d items instead of 2", items_written);
		fclose(f1);
		fclose(f2);
		goto cleanup;
	}

	fclose(f1);
	fclose(f2);

	/* Seed from first file, store results */

	srand_from_file(filename1);

	for (int i = 0; i < num_tests; i++)
		results[i] = rand();

	/* Seed from second file, test to make sure results are different */

	srand_from_file(filename2);

	num_identical = 0;

	for (int i = 0; i < num_tests; i++)
		if (results[i] == rand())
			num_identical++;

	if (num_identical >= num_tests) {
		warnx("First and second seeds do not differ");
		goto cleanup;
	}

	/* Seed from first file, test to make sure results are the same */

	srand_from_file(filename1);

	num_identical = 0;

	for (int i = 0; i < num_tests; i++)
		if (results[i] == rand())
			num_identical++;

	if (num_identical != num_tests) {
		warnx("First and third seeds have only %d matches.",
			num_identical);
		goto cleanup;
	}

	/* All tests passed if we reach this point */

	retval = 1;

cleanup:

	remove(filename1);
	remove(filename2);

	free(filename1);
	free(filename2);

	return retval;
}

void
test_rand_ascii(int num_chars)
{
	for (int i = 0; i < num_chars; i++) {
		char c = rand_ascii();
		putchar(c);
	}
	putchar('\n');
}

void
print_usage()
{
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "  %s [-c<number of characters> | -s]\n", progname);
}
