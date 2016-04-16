#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Color.h"
#include "Vect.h"
#include "Ray.h"

class Object
{
public:
	Object() {}

	Color color() const { return Color(0,0,0,0); }
	double find_intersection(const Ray & ray);
};

double Object::find_intersection(const Ray & ray)
{
	return 0;
}

#endif
