#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <cmath>

#include "Color.h"
#include "Vect.h"
#include "Object.h"

class Sphere : public Object
{
public:
	Sphere();
	Sphere(Vect center0, double r, Color color0) 
		: center_(center0), radius_(r), color_(color0) {}

	virtual Vect center() const { return center_; }
	virtual double radius() const { return radius_; }
	virtual Color color() const { return color_; }
	virtual double find_intersection(const Ray & ray) const;
	virtual Vect normal(const Vect & pt) const { return (pt - center_).normalize(); }//divide by radius faster?

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

double Sphere::find_intersection(const Ray & ray) const
{
	double b = 	(2*(ray.start().x() - center_.x()) * ray.direction().x()) + 
				(2*(ray.start().y() - center_.y()) * ray.direction().y()) + 
		 		(2*(ray.start().z() - center_.z()) * ray.direction().z());

	double c = 	pow(ray.start().x() - center_.x(), 2) + 
				pow(ray.start().y() - center_.y(), 2) +
				pow(ray.start().z() - center_.z(), 2) - radius_ * radius_;
	
	double discriminant = b*b - 4*c;
	if(discriminant > 0)
	{
		//the ray intersects the sphere (on both sides... find the closer one)
		//using quadratic formula, with a = 1
		double root1 = ((-1*b - sqrt(discriminant))/2) - 0.000001;
		if(root1 > 0) { return root1; }
		else 
		{ 
			double root2 = ((-1*b + sqrt(discriminant))/2) - 0.000001; // + 0.000001?
			return root2; 
		}
	}
	else //the ray misses the sphere
	{
		return -1;
	}
}

#endif
