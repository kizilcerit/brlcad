/*                     T E X T U R E _ G R A D I E N T . H
 * BRL-CAD
 *
 * Copyright (c) 2002-2007 United States Government as represented by
 * the U.S. Army Research Laboratory.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this file; see the file named COPYING for more
 * information.
 */
/**
 * @file texture_gradient.h
 *
 *  Comments -
 *      Texture Library - Gradient Header
 *
 *  Author -
 *      Justin L. Shumaker
 *
 *  Source -
 *      The U. S. Army Research Laboratory
 *      Aberdeen Proving Ground, Maryland  21005-5068  USA
 *
 * $Id$
 */

#ifndef _TEXTURE_GRADIENT_H
#define _TEXTURE_GRADIENT_H


#include "texture.h"


typedef struct texture_gradient_s {
  int		axis;
} texture_gradient_t;


extern	void	texture_gradient_init(texture_t *texture, int axis);
extern	void	texture_gradient_free(texture_t *texture);
extern	void	texture_gradient_work(texture_t *texture, common_mesh_t *mesh, tie_ray_t *ray, tie_id_t *id, TIE_3 *pixel);


#endif

/*
 * Local Variables:
 * mode: C
 * tab-width: 8
 * c-basic-offset: 4
 * indent-tabs-mode: t
 * End:
 * ex: shiftwidth=4 tabstop=8
 */
