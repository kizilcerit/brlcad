/*
 *			R E F R A C T
 *
 *  Authors -
 *	Michael John Muuss
 *  
 *  Source -
 *	SECAD/VLD Computing Consortium, Bldg 394
 *	The U. S. Army Ballistic Research Laboratory
 *	Aberdeen Proving Ground, Maryland  21005
 *  
 *  Copyright Notice -
 *	This software is Copyright (C) 1985,1987 by the United States Army.
 *	All rights reserved.
 */
#ifndef lint
static char RCSrefract[] = "@(#)$Header$ (BRL)";
#endif

#include "conf.h"

#include <stdio.h>
#include <math.h>
#include "machine.h"
#include "vmath.h"
#include "mater.h"
#include "raytrace.h"
#include "./rdebug.h"
#include "./material.h"

int	max_ireflect = 5;	/* Maximum internal reflection level */
int	max_bounces = 5;	/* Maximum recursion level */

#define MSG_PROLOGUE	20		/* # initial messages to see */
#define MSG_INTERVAL	4000		/* message interval thereafter */

#define RI_AIR		1.0		/* Refractive index of air */

#define AIR_GAP_TOL	0.01		/* Max permitted air gap for RI tracking */

HIDDEN int	rr_hit(), rr_miss();
HIDDEN int	rr_refract();

extern vect_t	background;

/*
 *			R R _ R E N D E R
 */
int
rr_render( ap, pp, swp )
register struct application *ap;
struct partition	*pp;
struct shadework	*swp;
{
	struct application sub_ap;
	vect_t	work;
	vect_t	incident_dir;
	vect_t	filter_color;
	fastf_t	shader_fract;
	vect_t	shader_color;
	fastf_t	reflect;
	vect_t	reflect_color;
	fastf_t	transmit;
	vect_t	transmit_color;
	fastf_t	attenuation;
	vect_t	to_eye;

	RT_AP_CHECK(ap);

	reflect = swp->sw_reflect;
	transmit = swp->sw_transmit;
	if( reflect <= 0 && transmit <= 0 )
		goto out;

	if( ap->a_level > max_bounces )  {
		/* Nothing more to do for this ray */
		static long count = 0;		/* Not PARALLEL, should be OK */

		if( (rdebug&(RDEBUG_SHOWERR|RDEBUG_REFRACT)) && (
			count++ < MSG_PROLOGUE ||
			(count%MSG_INTERVAL) == 3
		) )  {
			rt_log("rr_render: %d,%d MAX BOUNCES=%d: %s\n",
				ap->a_x, ap->a_y,
				ap->a_level,
				pp->pt_regionp->reg_name );
		}

		/* 
		 * Return the basic color of the object, ignoring the
		 * the fact that it is supposed to be
		 * filtering or reflecting light here.
		 * This is much better than returning just black,
		 * but something better might be done.
		 */
		VMOVE( swp->sw_color, swp->sw_basecolor );
		ap->a_cumlen += pp->pt_inhit->hit_dist;
		goto out;
	}
	VMOVE( filter_color, swp->sw_basecolor );

	if( (swp->sw_inputs & (MFI_HIT|MFI_NORMAL)) != (MFI_HIT|MFI_NORMAL) )
		shade_inputs( ap, pp, swp, MFI_HIT|MFI_NORMAL );

	/*
	 *  If this ray is being fired from the exit point of
	 *  an object, and is directly entering another object,
	 *  (ie, there is no intervening air-gap), and
	 *  the two refractive indices match, then do not fire a
	 *  reflected ray -- just take the transmission contribution.
	 *  This is important, eg, for glass gun tubes projecting
	 *  through a glass armor plate. :-)
	 */
	if( NEAR_ZERO( pp->pt_inhit->hit_dist, AIR_GAP_TOL ) &&
	    ap->a_refrac_index == swp->sw_refrac_index )  {
	    	transmit += reflect;
	    	reflect = 0;
	}

	/*
	 *  Diminish base color appropriately, and add in
	 *  contributions from mirror reflection & transparency
	 */
	shader_fract = 1 - (reflect + transmit);
	if( shader_fract < 0 )  {
		shader_fract = 0;
	} else if( shader_fract >= 1 )  {
		goto out;
	}
	if(rdebug&RDEBUG_REFRACT) {
		rt_log("rr_render: lvl=%d start shader=%g, reflect=%g, transmit=%g %s\n",
			ap->a_level,
			shader_fract, reflect, transmit,
			pp->pt_regionp->reg_name );
	}
	VSCALE( shader_color, swp->sw_color, shader_fract );

	/*
	 *  Compute transmission through an object.
	 *  There may be a mirror reflection, which will be handled
	 *  by the reflection code later
	 */
	if( transmit > 0 )  {
		if(rdebug&RDEBUG_REFRACT) {
			rt_log("rr_render: lvl=%d begin xmit through %s\n",
				ap->a_level,
				pp->pt_regionp->reg_name );
		}
		/*
		 *  Calculate refraction at entrance.
		 */
		sub_ap = *ap;		/* struct copy */
		sub_ap.a_level = 0;	/* # of internal reflections */
		sub_ap.a_cumlen = 0;	/* distance through the glass */
		sub_ap.a_user = -1;	/* sanity */
		sub_ap.a_uptr = (genptr_t)(pp->pt_regionp);
		VMOVE( sub_ap.a_ray.r_pt, swp->sw_hit.hit_point );
		VMOVE( incident_dir, ap->a_ray.r_dir );

		/* If there is an air gap, reset ray's RI to air */
		if( pp->pt_inhit->hit_dist > AIR_GAP_TOL )
			sub_ap.a_refrac_index = RI_AIR;

		if( sub_ap.a_refrac_index != swp->sw_refrac_index &&
		    !rr_refract( incident_dir,		/* input direction */
			swp->sw_hit.hit_normal,		/* exit normal */
			sub_ap.a_refrac_index,		/* current RI */
			swp->sw_refrac_index,		/* next RI */
			sub_ap.a_ray.r_dir		/* output direction */
		) )  {
			/*
			 *  Ray was mirror reflected back outside solid.
			 *  Just add contribution to reflection,
			 *  and quit.
			 */
			reflect += transmit;
			transmit = 0;
			VSETALL( transmit_color, 0 );
			if(rdebug&RDEBUG_REFRACT) {
				rt_log("rr_render: lvl=%d change xmit into reflection %s\n",
					ap->a_level,
					pp->pt_regionp->reg_name );
			}
			goto do_reflection;
		}

		/*
		 *  Find new exit point from the inside. 
		 *  We will iterate, but not recurse, due to the special
		 *  (non-recursing) hit and miss routines used here for
		 *  internal reflection.
		 *
		 *  a_onehit is set to 3, so that where possible,
		 *  rr_hit() will be given three accurate hit points:
		 *  the entry and exit points of this glass region,
		 *  and the entry point into the next region.
		 *  This permits calculation of the departing
		 *  refraction angle based on the RI of the current and
		 *  *next* regions along the ray.
		 */
do_inside:
		sub_ap.a_hit =  rr_hit;
		sub_ap.a_miss = rr_miss;
		sub_ap.a_purpose = "internal reflection";
		sub_ap.a_onehit = 3;
		switch( rt_shootray( &sub_ap ) )  {
		case 2:
			/* All is well, implicit returns stored in sub_ap */
			break;
		case 1:
			/* Treat as escaping ray */
			goto do_exit;
		case 0:
		default:
			/* Dreadful error */
			VSET( swp->sw_color, 0, 99, 0 ); /* very green */
			goto out;			/* abandon hope */
		}

		/* NOTE: rr_hit returns EXIT Point in sub_ap.a_uvec,
		 *  and returns EXIT Normal in sub_ap.a_vvec,
		 *  and returns next RI in sub_ap.a_refrac_index
		 */
		if( rdebug&RDEBUG_RAYWRITE )  {
			wraypts( sub_ap.a_ray.r_pt,
				sub_ap.a_ray.r_dir,
				sub_ap.a_uvec,
				2, ap, stdout );	/* 2 = ?? */
		}
		if( rdebug&RDEBUG_RAYPLOT )  {
			pl_color( stdout, 0, 255, 0 );
			pdv_3line( stdout,
				sub_ap.a_ray.r_pt,
				sub_ap.a_uvec );
		}
		VMOVE( sub_ap.a_ray.r_pt, sub_ap.a_uvec );
		VMOVE( incident_dir, sub_ap.a_ray.r_dir );

		/*
		 *  Calculate refraction at exit.
		 *  Use "look ahead" RI value from rr_hit.
		 */
		if( !rr_refract( incident_dir,		/* input direction */
			sub_ap.a_vvec,			/* exit normal */
			swp->sw_refrac_index,		/* current RI */
			sub_ap.a_refrac_index,		/* next RI */
			sub_ap.a_ray.r_dir		/* output direction */
		) )  {
			static long count = 0;		/* not PARALLEL, should be OK */

			/* Reflected internally -- keep going */
			if( (++sub_ap.a_level) <= max_ireflect )
				goto do_inside;

			/*
			 *  Internal Reflection limit exceeded -- just let
			 *  the ray escape, continuing on current course.
			 *  This will cause some energy from somewhere in the
			 *  sceen to be received through this glass,
			 *  which is much better than just returning
			 *  grey or black, as before.
			 */
			if( (rdebug&(RDEBUG_SHOWERR|RDEBUG_REFRACT)) && (
				count++ < MSG_PROLOGUE ||
				(count%MSG_INTERVAL) == 3
			) )  {
				rt_log("rr_render: %d,%d Int.reflect=%d: %s lvl=%d\n",
					sub_ap.a_x, sub_ap.a_y,
					sub_ap.a_level,
					pp->pt_regionp->reg_name,
					ap->a_level );
			}
			VMOVE( sub_ap.a_ray.r_dir, incident_dir );
			goto do_exit;
		}
do_exit:
		/*
		 *  Compute internal spectral transmittance.
		 *  Bouger's law.  pg 30 of "color science"
		 *
		 *  Apply attenuation factor due to thickness of the glass.
		 *  sw_extinction is in terms of fraction of light absorbed
		 *  per linear meter of glass.  a_cumlen is in mm.
		 */
		if( swp->sw_extinction > 0 && sub_ap.a_cumlen > 0 )  {
			attenuation = pow( 10.0, -1.0e-3 * sub_ap.a_cumlen *
				swp->sw_extinction );
		} else {
			attenuation = 1;
		}

		/*
		 *  Process the escaping refracted ray.
		 *  This is the only place we might recurse dangerously,
		 *  so we are careful to use our caller's recursion level+1.
		 *  NOTE: point & direction already filled in
		 */
		sub_ap.a_hit =  ap->a_hit;
		sub_ap.a_miss = ap->a_miss;
		sub_ap.a_onehit = ap->a_onehit;
		sub_ap.a_level = ap->a_level+1;
		sub_ap.a_purpose = "escaping refracted ray";
		sub_ap.a_refrac_index = swp->sw_refrac_index;
		sub_ap.a_cumlen = 0;
		(void) rt_shootray( &sub_ap );

		/* a_user has hit/miss flag! */
		if( sub_ap.a_user == 0 )  {
			VMOVE( transmit_color, background );
			sub_ap.a_cumlen = 0;
		} else {
			VMOVE( transmit_color, sub_ap.a_color );
		}
		transmit *= attenuation;
		VELMUL( transmit_color, filter_color, transmit_color );
		if(rdebug&RDEBUG_REFRACT) {
			rt_log("rr_render: lvl=%d end of xmit through %s\n",
				ap->a_level,
				pp->pt_regionp->reg_name );
		}
	} else {
		VSETALL( transmit_color, 0 );
	}

	/*
	 *  Handle any reflection, including mirror reflections
	 *  detected by the transmission code, above.
	 */
do_reflection:
	if( reflect > 0 )  {
		register fastf_t	f;

		/* Mirror reflection */
		sub_ap = *ap;		/* struct copy */
		sub_ap.a_level = ap->a_level+1;
		sub_ap.a_onehit = 1;
		VMOVE( sub_ap.a_ray.r_pt, swp->sw_hit.hit_point );
		VREVERSE( to_eye, ap->a_ray.r_dir );
		f = 2 * VDOT( to_eye, swp->sw_hit.hit_normal );
		VSCALE( work, swp->sw_hit.hit_normal, f );
		/* I have been told this has unit length */
		VSUB2( sub_ap.a_ray.r_dir, work, to_eye );
		sub_ap.a_purpose = "reflected ray";

		if( rdebug&RDEBUG_RAYPLOT )  {
			point_t		endpt;
			/* Plot the surface normal */
			pl_color( stdout, 0, 255, 0 );
			f = sub_ap.a_rt_i->rti_radius * 0.02;
			VJOIN1( endpt, sub_ap.a_ray.r_pt,
				f, swp->sw_hit.hit_normal );
			pdv_3line( stdout, sub_ap.a_ray.r_pt, endpt );
		}

		(void)rt_shootray( &sub_ap );

		/* a_user has hit/miss flag! */
		if( sub_ap.a_user == 0 )  {
			/* MISS */
			VMOVE( reflect_color, background );
		} else {
			ap->a_cumlen += sub_ap.a_cumlen;
			VMOVE( reflect_color, sub_ap.a_color );
		}
	} else {
		VSETALL( reflect_color, 0 );
	}

	/*
	 *  Collect the contributions to the final color
	 */
	VJOIN2( swp->sw_color, shader_color,
		reflect, reflect_color,
		transmit, transmit_color );
	if(rdebug&RDEBUG_REFRACT)  {
		rt_log("rr_render: lvl=%d end shader=%g reflect=%g, transmit=%g %s\n",
			ap->a_level,
			shader_fract, reflect, transmit,
			pp->pt_regionp->reg_name );
		VPRINT("shader  ", shader_color);
		VPRINT("reflect ", reflect_color);
		VPRINT("transmit", transmit_color);
	}
out:
	if(rdebug&RDEBUG_REFRACT)  {
		VPRINT("final   ", swp->sw_color);
	}
	return(1);
}

/*
 *			R R _ M I S S
 */
HIDDEN int
/*ARGSUSED*/
rr_miss( ap, PartHeadp )
register struct application *ap;
struct partition *PartHeadp;
{
	RT_AP_CHECK(ap);
	return(1);	/* treat as escaping ray */
}

/*
 *			R R _ H I T
 *
 *  This routine is called when an internal reflection ray hits something
 *  (which is ordinarily the case).
 *
 *  Generally, there will be one or two partitions on the hit list.
 *  The values for pt_outhit for the second partition should not be used,
 *  as a_onehit was set to 3, getting a maximum of 3 valid hit points.
 *
 *  Explicit Returns -
 *	0	dreadful internal error
 *	1	treat as escaping ray & reshoot
 *	2	Proper exit point determined, with Implicit Returns:
 *			a_uvec		exit Point
 *			a_vvec		exit Normal (inward pointing)
 *			a_refrac_index	RI of *next* material
 */
HIDDEN int
rr_hit( ap, PartHeadp )
register struct application *ap;
struct partition *PartHeadp;
{
	register struct partition *pp;
	register struct hit	*hitp;
	register struct soltab	*stp;
	struct partition	*psave;				
	struct shadework	sw;
	struct application	appl;

	RT_AP_CHECK(ap);

	for( pp=PartHeadp->pt_forw; pp != PartHeadp; pp = pp->pt_forw )
		if( pp->pt_outhit->hit_dist > 0.0 )  break;
	if( pp == PartHeadp )  {
		if(rdebug&(RDEBUG_SHOWERR|RDEBUG_REFRACT))  {
			rt_log("rr_hit:  %d,%d no hit out front?\n",
				ap->a_x, ap->a_y );
			return(0);	/* error */
		}
		return(1);		/* treat as escaping ray */
	}

	/*
	 *  Ensure that the partition we are given is part of the same
	 *  region that we started in.  When the internal reflection
	 *  is happening very near an edge or corner, this is not always
	 *  the case, and either (a) a small sliver of some other region
	 *  is found to be in the way, or (b) the ray completely misses the
	 *  region that it started in, although not by much.
	 */
	psave = pp;
	for( ; pp != PartHeadp; pp = pp->pt_forw )
		if( pp->pt_regionp == (struct region *)(ap->a_uptr) )  break;
	if( pp == PartHeadp )  {
		if(rdebug&(RDEBUG_SHOWERR|RDEBUG_REFRACT))  {
			rt_log("rr_hit:  %d,%d Ray int.reflected in %s landed in %s\n",
				ap->a_x, ap->a_y,
				((struct region *)(ap->a_uptr))->reg_name,
				psave->pt_regionp->reg_name );
			return(0);	/* error */
		}
		return(1);		/* treat as escaping ray */
	}

	/*
	 *  At one time, this was a check for pp->pt_inhit->hit_dist
	 *  being NEAR zero.  That was a mistake, because we may have
	 *  been at the edge of a subtracted out center piece when
	 *  internal reflection happened, except that floating point
	 *  error (being right on the surface of the interior solid)
	 *  prevented us from "seeing" that solid on the next ray,
	 *  causing our ray endpoints to be quite far from the starting
	 *  point, yet with the ray still validly inside the glass region.
	 *
	 *  There is a major problem if the entry point
	 *  is further ahead than the firing point, ie, >0.
	 *
	 *  Because this error has not yet been encountered, it is
	 *  considered dreadful.  Some recovery may be possible.
	 */
	if( pp->pt_inhit->hit_dist > 10 )  {
		stp = pp->pt_inseg->seg_stp;
		rt_log("rr_hit: %d,%d %s inhit %g > 10.0!\n",
			ap->a_x, ap->a_y,
			pp->pt_regionp->reg_name,
			pp->pt_inhit->hit_dist);
		return(0);		/* dreadful error */
	}

	/*
	 * If there is a very small crack in the glass, perhaps formed
	 * by a small error when taking the Union of two solids,
	 * attempt to find the real exit point.
	 * NOTE that this is usually taken care of inside librt
	 * in the bool_weave code, but it is inexpensive to check for it
	 * here.  If this case is detected, push on, and log it.
	 * This code is not expected to be needed.
	 */
	while( pp->pt_forw != PartHeadp )  {
		register fastf_t d;
		d = pp->pt_forw->pt_inhit->hit_dist - pp->pt_outhit->hit_dist;
		if( !NEAR_ZERO( d, AIR_GAP_TOL ) )
			break;
		if( pp->pt_forw->pt_regionp != pp->pt_regionp )
			break;
		if(rdebug&(RDEBUG_SHOWERR|RDEBUG_REFRACT)) rt_log(
			"rr_hit: %d,%d fusing small crack in glass %s\n",
			ap->a_x, ap->a_y,
			pp->pt_regionp->reg_name );
		pp = pp->pt_forw;
	}

	hitp = pp->pt_outhit;
	stp = pp->pt_outseg->seg_stp;
	if( hitp->hit_dist >= INFINITY )  {
		rt_log("rr_hit: %d,%d infinite glass (%g,%g) %s\n",
			ap->a_x, ap->a_y,
			pp->pt_inhit->hit_dist, hitp->hit_dist,
			pp->pt_regionp->reg_name );
		return(0);		/* dreadful error */
	}
	VJOIN1( hitp->hit_point, ap->a_ray.r_pt,
		hitp->hit_dist, ap->a_ray.r_dir );
	RT_HIT_NORM( hitp, stp, &(ap->a_ray) );
	if( pp->pt_outflip )  {
		VREVERSE( hitp->hit_normal, hitp->hit_normal );
	}

	/*
	 *  Look ahead, and see if there is more glass to come.
	 *  If so, obtain its refractive index, to enable correct
	 *  calculation of the departing refraction angle.
	 */
	ap->a_refrac_index = RI_AIR;			/* Default medium: air */
	if( pp->pt_forw != PartHeadp )  {
		register fastf_t	d;
		d = pp->pt_forw->pt_inhit->hit_dist - hitp->hit_dist;
		if( NEAR_ZERO( d, AIR_GAP_TOL ) )  {
			/*
			 * Make a private copy of the application struct,
			 * because viewshade() may change various fields.
			 */
			appl = *ap;			/* struct copy */

			bzero( (char *)&sw, sizeof(sw) );
			sw.sw_transmit = sw.sw_reflect = 0.0;
			sw.sw_refrac_index = 1.0;
			sw.sw_xmitonly = 1;		/* want XMIT data only */
			sw.sw_inputs = 0;		/* no fields filled yet */
			VSETALL( sw.sw_color, 1 );
			VSETALL( sw.sw_basecolor, 1 );

			(void)viewshade( &appl, pp->pt_forw, &sw );
			ap->a_refrac_index = sw.sw_refrac_index;
		}
	}

	/* For refraction, want exit normal to point inward. */
	VREVERSE( ap->a_vvec, hitp->hit_normal );
	VMOVE( ap->a_uvec, hitp->hit_point );
	ap->a_cumlen += (hitp->hit_dist - pp->pt_inhit->hit_dist);
	return(2);			/* OK */
}

/*
 *			R E F R A C T
 *
 *	Compute the refracted ray 'v_2' from the incident ray 'v_1' with
 *	the refractive indices 'ri_2' and 'ri_1' respectively.
 *	Using Schnell's Law:
 *
 *		theta_1 = angle of v_1 with surface normal
 *		theta_2 = angle of v_2 with reversed surface normal
 *		ri_1 * sin( theta_1 ) = ri_2 * sin( theta_2 )
 *
 *		sin( theta_2 ) = ri_1/ri_2 * sin( theta_1 )
 *		
 *	The above condition is undefined for ri_1/ri_2 * sin( theta_1 )
 *	being greater than 1, and this represents the condition for total
 *	reflection, the 'critical angle' is the angle theta_1 for which
 *	ri_1/ri_2 * sin( theta_1 ) equals 1.
 *
 *  Returns TRUE if refracted, FALSE if reflected.
 *
 *  Note:  output (v_2) can be same storage as an input.
 */
HIDDEN int
rr_refract( v_1, norml, ri_1, ri_2, v_2 )
register vect_t	v_1;
register vect_t	norml;
double	ri_1, ri_2;
register vect_t	v_2;
{
	LOCAL vect_t	w, u;
	FAST fastf_t	beta;

	if( NEAR_ZERO(ri_1, 0.0001) || NEAR_ZERO( ri_2, 0.0001 ) )  {
		rt_log("rr_refract:ri1=%g, ri2=%g\n", ri_1, ri_2 );
		beta = 1;
	} else {
		beta = ri_1/ri_2;		/* temp */
		if( beta > 10000 )  {
			rt_log("rr_refract:  beta=%g\n", beta);
			beta = 1000;
		}
	}
	VSCALE( w, v_1, beta );
	VCROSS( u, w, norml );
	    	
	/*
	 *	|w X norml| = |w||norml| * sin( theta_1 )
	 *	        |u| = ri_1/ri_2 * sin( theta_1 ) = sin( theta_2 )
	 */
	if( (beta = VDOT( u, u )) > 1.0 )  {
		/*  Past critical angle, total reflection.
		 *  Calculate reflected (bounced) incident ray.
		 */
		if(rdebug&RDEBUG_REFRACT) rt_log("rr_refract: reflected.  ri1=%g ri2=%g beta=%g\n",
			ri_1, ri_2, beta);
		VREVERSE( u, v_1 );
		beta = 2 * VDOT( u, norml );
		VSCALE( w, norml, beta );
		VSUB2( v_2, w, u );
		return(0);		/* reflected */
	} else {
		/*
		 * 1 - beta = 1 - sin( theta_2 )^^2
		 *	    = cos( theta_2 )^^2.
		 *     beta = -1.0 * cos( theta_2 ) - Dot( w, norml ).
		 */
		if(rdebug&RDEBUG_REFRACT) rt_log("rr_refract: refracted.  ri1=%g ri2=%g beta=%g\n",
			ri_1, ri_2, beta);
		beta = -sqrt( 1.0 - beta) - VDOT( w, norml );
		VSCALE( u, norml, beta );
		VADD2( v_2, w, u );		
		return(1);		/* refracted */
	}
	/* NOTREACHED */
}
