/*
 *			I G E S - G / M A I N . C
 *
 *  Authors -
 *	John R. Anderson
 *	Susanne L. Muuss
 *	Earl P. Weaver
 *
 *  Source -
 *	VLD/ASB Building 1065
 *	The U. S. Army Ballistic Research Laboratory
 *	Aberdeen Proving Ground, Maryland  21005
 *  
 *  Copyright Notice -
 *	This software is Copyright (C) 1990 by the United States Army.
 *	All rights reserved.
 */
#ifndef lint
static char RCSid[] = "@(#)$Header$ (BRL)";
#endif

#include <stdio.h>
#include "machine.h"
#include "vmath.h"
#include "./iges_struct.h"
#include "./iges_types.h"

char eor,eof,card[256];
fastf_t scale,inv_scale,conv_factor;
int units,counter,pstart,dstart,totentities,dirarraylen;
FILE *fd,*fdout;
char brlcad_file[256];
int reclen,currec,ntypes;
int brlcad_att_de=0;
struct iges_directory **dir;
struct reglist *regroot;
struct iges_edge_list *edge_root;
struct iges_vertex_list *vertex_root;

char operator[]={
	' ',
	'u',
	'+',
	'-' };

mat_t *identity;
extern int errno;

extern char	version[];

main( argc , argv )
int argc;
char *argv[];
{
	int i;

	if( argc != 3 )
		usage();

	printf( "%s", version+5);
	printf( "Please direct bug reports to <jra@brl.mil>\n\n" );

	ntypes = NTYPES;
	regroot = NULL;
	edge_root = NULL;
	vertex_root = NULL;

	fd = fopen( *++argv , "r" );	/* open IGES file */
	if( fd == NULL )
	{
		fprintf( stderr , "Cannot open %s\n" , *argv );
		perror( "iges-g" );
		exit( 1 );
	}

	reclen = Recsize() * sizeof( char ); /* Check length of records */
	if( reclen == 0 )
	{
		fprintf( stderr , "File not in IGES ASCII format\n" );
		exit(1);
	}

	strcpy( brlcad_file ,  *++argv );

	fdout = fopen( brlcad_file , "w" );	/* open BRLCAD file */
	if( fdout == NULL )
	{
		fprintf( stderr , "Cannot open %s\n" , brlcad_file );
		perror( "iges-g" );
		exit( 1 );
	}

	identity = (mat_t *)malloc( sizeof( mat_t ) );
	for( i=0 ; i<16 ; i++ )
	{
		if( !(i%5) )
			(*identity)[i] = 1.0;
		else
			(*identity)[i] = 0.0;
	}

	Initstack();	/* Initialize node stack */

	Readstart();	/* Read start section */

	Readglobal();	/* Read global section */

	pstart = Findp();	/* Find start of parameter section */

	Makedir();	/* Read directory section and build a linked list of entries */

	Summary();	/* Print a summary of what is in the IGES file */

	Docolor();	/* Get color info from color definition entities */

	Get_att();	/* Look for a BRLCAD attribute definition */

	Evalxform();	/* Accumulate the transformation matrices */

	Check_names();	/* Look for name entities */

	Convinst();	/* Handle Instances */

	Convsolids();	/* Convert solid entities */

	Convsurfs();	/* Convert NURBS */

	Convtree();	/* Convert Boolean Trees */

	Convassem();	/* Convert solid assemblies */

}
