/*                 RectangularTrimmedSurface.cpp
 * BRL-CAD
 *
 * Copyright (c) 1994-2009 United States Government as represented by
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
/** @file RectangularTrimmedSurface.cpp
 *
 * Routines to interface to STEP "RectangularTrimmedSurface".
 *
 */

#include "STEPWrapper.h"
#include "Factory.h"

#include "Surface.h"
#include "RectangularTrimmedSurface.h"

#define CLASSNAME "RectangularTrimmedSurface"
#define ENTITYNAME "Rectangular_Trimmed_Surface"
string RectangularTrimmedSurface::entityname = Factory::RegisterClass(ENTITYNAME,(FactoryMethod)RectangularTrimmedSurface::Create);

RectangularTrimmedSurface::RectangularTrimmedSurface() {
	step=NULL;
	id = 0;
	basis_surface = NULL;
}

RectangularTrimmedSurface::RectangularTrimmedSurface(STEPWrapper *sw,int STEPid) {
	step=sw;
	id = STEPid;
	basis_surface = NULL;
}

RectangularTrimmedSurface::~RectangularTrimmedSurface() {
}

bool
RectangularTrimmedSurface::Load(STEPWrapper *sw, SCLP23(Application_instance) *sse) {

	step=sw;
	id = sse->STEPfile_id;

	if ( !BoundedSurface::Load(step,sse) ) {
		cout << CLASSNAME << ":Error loading base class ::BoundedSurface." << endl;
		return false;
	}

	// need to do this for local attributes to makes sure we have
	// the actual entity and not a complex/supertype parent
	sse = step->getEntity(sse,ENTITYNAME);

	if (basis_surface == NULL) {
		SCLP23(Application_instance) *entity = step->getEntityAttribute(sse,"basis_surface");
		if (entity) {
			basis_surface = dynamic_cast<Surface *>(Factory::CreateObject(sw,entity));
		} else {
			cerr << CLASSNAME << ": error loading 'basis_surface' attribute." << endl;
			return false;
		}
	}

	u1 = step->getRealAttribute(sse,"u1");
	u2 = step->getRealAttribute(sse,"u2");
	v1 = step->getRealAttribute(sse,"v1");
	v2 = step->getRealAttribute(sse,"v2");

	usense = step->getBooleanAttribute(sse,"usense");
	vsense = step->getBooleanAttribute(sse,"vsense");

	return true;
}

void
RectangularTrimmedSurface::Print(int level) {
	TAB(level); cout << CLASSNAME << ":" << name << "(";
	cout << "ID:" << STEPid() << ")" << endl;

	TAB(level); cout << "Attributes:" << endl;
	basis_surface->Print(level+1);

	TAB(level+1); cout << "u1:" << u1 << endl;
	TAB(level+1); cout << "u2:" << u2 << endl;
	TAB(level+1); cout << "v1:" << u1 << endl;
	TAB(level+1); cout << "v2:" << u2 << endl;

	TAB(level+1); cout << "usense:" << step->getBooleanString((SCLBOOL_H(Bool))usense) << endl;
	TAB(level+1); cout << "vsense:" << step->getBooleanString((SCLBOOL_H(Bool))vsense) << endl;

	TAB(level); cout << "Inherited Attributes:" << endl;
	BoundedSurface::Print(level+1);
}

STEPEntity *
RectangularTrimmedSurface::Create(STEPWrapper *sw, SCLP23(Application_instance) *sse) {
	Factory::OBJECTS::iterator i;
	if ((i = Factory::FindObject(sse->STEPfile_id)) == Factory::objects.end()) {
		RectangularTrimmedSurface *object = new RectangularTrimmedSurface(sw,sse->STEPfile_id);

		Factory::AddObject(object);

		if (!object->Load(sw, sse)) {
			cerr << CLASSNAME << ":Error loading class in ::Create() method." << endl;
			delete object;
			return NULL;
		}
		return static_cast<STEPEntity *>(object);
	} else {
		return (*i).second;
	}
}

bool
RectangularTrimmedSurface::LoadONBrep(ON_Brep *brep)
{
	cerr << "Error: ::LoadONBrep(ON_Brep *brep) not implemented for " << entityname << endl;
	return false;
}

