#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Color.h"
#include "Vect.h"
#include "Object.h"

class Sphere : public Object
{
public:
	Sphere();
	Sphere(Vect center0, double r, Color color0) 
		: center_(center0), radius_(r), color_(color0) {}

	Vect center() const { return center_; }
	double radius() const { return radius_; }
	Color color() const { return color_; }

private:
	Vect center_;
	double radius_;
	Color color_;
};

Sphere::Sphere()
{
	center_ = Vect(0,0,0);
	radius_ = 1;
	color_ = Color(0.5,0.5,0.5,0);
}

#endif
