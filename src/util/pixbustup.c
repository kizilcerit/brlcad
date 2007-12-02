/*                     P I X B U S T U P . C
 * BRL-CAD
 *
 * Copyright (c) 1986-2007 United States Government as represented by
 * the U.S. Army Research Laboratory.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; see the file named COPYING for more
 * information.
 */
/** @file pixbustup.c
 *
 *	Take concatenated .pix files, and write them into individual files.
 *	Mostly a holdover from the days when RT wrote animations into
 *	one huge file, but still occasionally useful.
 *
 *  Author -
 *	Michael John Muuss
 *
 */
#ifndef lint
static const char RCSid[] = "@(#)$Header$ (BRL)";
#endif

#include "common.h"

#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif

#include "machine.h"


static int scanbytes;			/* # of bytes of scanline */

unsigned char *in1;

static int nlines;		/* Number of input lines */
static int pix_line;		/* Number of pixels/line */

char usage[] =
"Usage: pixbustup basename width [image_offset] [first_number] <input.pix\n";

int infd;

int
main(int argc, char **argv)
{
	int image_offset;
	int framenumber;
	char *base_name;
	char name[128];

	if( argc < 3 )  {
		fprintf(stderr,"%s", usage);
		exit(1);
	}

	base_name = argv[1];
	nlines = atoi(argv[2] );

	pix_line = nlines;	/* Square pictures */
	scanbytes = nlines * pix_line * 3;
	in1 = (unsigned char  *) malloc( scanbytes );

	if( argc == 4 )  {
		image_offset = atoi(argv[3]);
		lseek(0, image_offset*scanbytes, 0);
	}
	if( argc == 5 )
		framenumber = atoi(argv[4]);
	else
		framenumber = 0;

	for( ; ; framenumber++ )  {
		int fd;
		int rwval = read( 0, in1, scanbytes );

		if( rwval != scanbytes ) {
		    if (rwval < 0) {
			perror("pixbustup READ ERROR");
		    }
		    break;
		}
		snprintf(name, 128, "%s.%d", base_name, framenumber);
		if( (fd=creat(name,0444))<0 )  {
			perror(name);
			continue;
		}
		rwval = write( fd, in1, scanbytes );
		if (rwval != scanbytes ) {
		    if (rwval < 0) {
			perror("pixbustup WRITE ERROR");
		    }
		}
		(void)close(fd);
		printf("wrote %s\n", name);
	}
	exit(0);
}

/*
 * Local Variables:
 * mode: C
 * tab-width: 8
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * ex: shiftwidth=4 tabstop=8
 */
