/*                 ProductRelatedProductCategory.h
 * BRL-CAD
 *
 * Copyright (c) 1994-2012 United States Government as represented by
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
/** @file step/ProductRelatedProductCategory.h
 *
 * Class definition used to convert STEP "ProductRelatedProductCategory" to BRL-CAD BREP
 * structures.
 *
 */

#ifndef PRODUCT_RELATED_PRODUCT_CATEGORY_H_
#define PRODUCT_RELATED_PRODUCT_CATEGORY_H_

#include "STEPEntity.h"

#include "sdai.h"

#include "ProductCategory.h"

// forward declaration of class
class ON_Brep;
class Product;
typedef list<Product *> LIST_OF_PRODUCTS;

class ProductRelatedProductCategory: public ProductCategory {
private:
    static string entityname;

protected:
    LIST_OF_PRODUCTS products;

public:
    ProductRelatedProductCategory();
    virtual ~ProductRelatedProductCategory();
    ProductRelatedProductCategory(STEPWrapper *sw, int step_id);
    bool Load(STEPWrapper *sw, SDAI_Application_instance *sse);
    virtual bool LoadONBrep(ON_Brep *brep);
    string ClassName();
    virtual void Print(int level);

    //static methods
    static STEPEntity *Create(STEPWrapper *sw, SDAI_Application_instance *sse);
};

#endif /* PRODUCT_RELATED_PRODUCT_CATEGORY_H_ */

/*
 * Local Variables:
 * tab-width: 8
 * mode: C
 * indent-tabs-mode: t
 * c-file-style: "stroustrup"
 * End:
 * ex: shiftwidth=4 tabstop=8
 */
