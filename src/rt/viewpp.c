/*                        V I E W P P . C
 * BRL-CAD
 *
 * Copyright (c) 1985-2014 United States Government as represented by
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
/** @file rt/viewpp.c
 *
 * Ray Tracing program RTPP bottom half.
 *
 * This module takes the first hit from rt_shootray(), and produces
 * GIFT format .PP (pretty picture) files
 *
 * To make a picture out of a .PP file, use:
 * pp-ik or
 * pp-fb
 *
 */

#include "common.h"

#include <stdio.h>

#include "vmath.h"
#include "raytrace.h"

#include "./ext.h"


const char title[] = "RT Pretty Picture";

void
usage(const char *argv0)
{
    bu_log("Usage:  %s [options] model.g objects... >file.pp\n", argv0);
    bu_log("Options:\n");
    bu_log(" -s #		Grid size in pixels, default 512, max 1024\n");
    bu_log(" -a Az		Azimuth in degrees	(conflicts with -M)\n");
    bu_log(" -e Elev	Elevation in degrees	(conflicts with -M)\n");
    bu_log(" -M		Read model2view matrix on stdin (conflicts with -a, -e)\n");
    bu_log(" -x #		Set librt debug flags\n");
}


/* Viewing module specific "set" variables */
struct bu_structparse view_parse[] = {
    {"",	0, (char *)0,	0,	BU_STRUCTPARSE_FUNC_NULL, NULL, NULL}
};

/* Stuff for pretty-picture output format */
static struct soltab *last_solidp;	/* pointer to last solid hit */
static int last_item;			/* item number of last region hit */
static int last_ihigh;			/* last inten_high */
static int ntomiss;			/* number of pixels to miss */
static int col;				/* column; for PP 75 char/line crap */

void view_pixel(void) {}

#define pchar(c) {putc(c, stdout);if (col++==74) {putc('\n', stdout);col=0;}}

/*
 * Oddball 5-bits in a char ('@', 'A', ... on up) number packing.
 * Number is written 5 bits at a time, right to left (low to high)
 * until there are no more non-zero bits remaining.
 */
void
pknum(int arg)
{
    register long i = arg;

    do {
	pchar((int)('@'+(i & 037)));
	i >>= 5;
    } while (i > 0);
}

/* Support for pretty-picture files */
int
pphit(register struct application *ap, struct partition *PartHeadp, struct seg *UNUSED(segHeadp))
{
    register struct partition *pp;
    register struct hit *hitp;
    double cosI0;
    register int i, j;
    vect_t normal;

    for (pp=PartHeadp->pt_forw; pp != PartHeadp; pp = pp->pt_forw)
	if (pp->pt_outhit->hit_dist >= 0.0) break;
    if (pp == PartHeadp) {
	bu_log("pphit:  no hit out front?\n");
	return 0;
    }
    hitp = pp->pt_inhit;
    RT_HIT_NORMAL(normal, hitp, pp->pt_inseg->seg_stp, &(ap->a_ray), pp->pt_inflip);

    cosI0 = -VDOT(normal, ap->a_ray.r_dir);
    if (pp->pt_inflip)
	cosI0 = -cosI0;
    if (cosI0 <= 0.0) {
	ntomiss++;
	return 0;
    }
    if (ntomiss > 0) {
	pchar(' ');		/* miss target cmd */
	pknum(ntomiss);
	ntomiss = 0;
	last_solidp = SOLTAB_NULL;
    }
    if (last_item != pp->pt_regionp->reg_regionid) {
	last_item = pp->pt_regionp->reg_regionid;
	pchar('#');		/* new item cmd */
	pknum(last_item);
	last_solidp = SOLTAB_NULL;
    }
    if (last_solidp != pp->pt_inseg->seg_stp) {
	last_solidp = pp->pt_inseg->seg_stp;
	pchar('!');		/* new solid cmd */
    }
    i = cosI0 * 255.0;		/* integer angle */
    j = (i>>5) & 07;
    if (j != last_ihigh) {
	last_ihigh = j;
	pchar('0'+j);		/* new inten high */
    }
    j = i & 037;
    pchar('@'+j);		/* low bits of pixel */
    return 1;
}

int
ppmiss(struct application *UNUSED(ap))
{
    last_solidp = SOLTAB_NULL;
    ntomiss++;
    return 0;
}

void
view_eol(void)
{
    pchar('.');		/* End of scanline */
    last_solidp = SOLTAB_NULL;
    ntomiss = 0;
}

/*
 * Called when the picture is finally done.
 */
void
view_end(void)
{
    fprintf(stdout, "/\n");	/* end of view */
    fflush(stdout);
}

int
view_init(register struct application *ap, char *file, char *obj, int minus_o)
{
    ap->a_hit = pphit;
    ap->a_miss = ppmiss;
    ap->a_onehit = 1;

    if (!minus_o)
	fprintf(stderr, "Warning:  -o ignored, .PP goes to stdout\n");

    fprintf(stdout, "%s: %s (RT)\n", file, obj);
    fprintf(stdout, "%10d%10d", (int)azimuth, (int)elevation);
    fprintf(stdout, "%10lu%10lu\n", (unsigned long int)width, (unsigned long int)height);
    return 0;		/* no framebuffer needed */
}

void view_2init(void) {;}

void view_setup(void) {}
void view_cleanup(void) {}

void application_init (void) {}

/*
 * Local Variables:
 * mode: C
 * tab-width: 8
 * indent-tabs-mode: t
 * c-file-style: "stroustrup"
 * End:
 * ex: shiftwidth=4 tabstop=8
 */
