/*
 *			M A T H T A B . H
 *
 *  Definitions to implement fast (but approximate) math library functions
 *  using table lookups.
 *
 *  Authors -
 *	Philip Dykstra
 *	Michael John Muuss
 *  
 *  Source -
 *	SECAD/VLD Computing Consortium, Bldg 394
 *	The U. S. Army Ballistic Research Laboratory
 *	Aberdeen Proving Ground, Maryland  21005
 *  
 *  Copyright Notice -
 *	This software is Copyright (C) 1985 by the United States Army.
 *	All rights reserved.
 *
 *  $Header$
 */
extern double pi, twopi, halfpi;

#define RANDTABSIZE	2048
#define	SINTABSIZE	2048

extern float *rand_ptr;
extern float rand_tab[];

extern double sin_scale;
extern float sin_table[];

#ifdef BENCHMARK
#define rand_half()	0
#define rand0to1()	0.5
#else
#define rand_half()	\
	(++rand_ptr >= &rand_tab[RANDTABSIZE] ? \
		*(rand_ptr = rand_tab) : *rand_ptr)

#define rand0to1()	(rand_half()+0.5)
#endif

#define tab_sin(a)	(((a) > 0) ? \
	( sin_table[(int)((0.5 + (a) * sin_scale))%SINTABSIZE] ) :\
	(-sin_table[(int)((0.5 - (a) * sin_scale))%SINTABSIZE] ) )
