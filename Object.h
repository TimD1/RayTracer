#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Color.h"
#include "Vect.h"
#include "Ray.h"

class Object
{
public:
	Object() {}

	virtual Color color() const { return Color(0,0,0,0); }
	virtual double find_intersection(const Ray & ray) const { return -2; }
	virtual Vect normal(const Vect & int_pos) const { return Vect(0,0,0); }
};

#endif
