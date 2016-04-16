#ifndef _PLANE_H_
#define _PLANE_H_

#include "Color.h"
#include "Vect.h"
#include "Object.h"

class Plane : public Object
{
public:
	Plane();
	Plane(Vect normal0, double d, Color color0) 
		: normal_(normal0), distance_(d), color_(color0) {}

	Vect normal() const { return normal_; }			//orientation
	Vect normal(Vect pt) const { return normal_; } 	//unnecessary?
	double distance() const { return distance_; }	//absolute position from origin
	Color color() const { return color_; }

	double find_intersection(const Ray & ray); //dist from camera along ray vec to pt

private:
	Vect normal_;
	double distance_;
	Color color_;
};

Plane::Plane()
{
	normal_ = Vect(1,0,0);
	distance_ = 0;
	color_ = Color(0.5,0.5,0.5,0);
}

double Plane::find_intersection(const Ray & ray)
{
	// find vector pointing directly from the plane to the origin
	Vect plane_to_origin( (normal_* distance_).invert() );

	// add position vector for where the ray starts
	// the result is a vector of the distance from ray to plane
	Vect plane_to_ray( plane_to_origin + ray.start() );

	// dist away relative to plane's orientation
	double dist_away = -(normal_ * plane_to_ray);

	// find how quickly ray travels in direction of plane's normal vector
	double velocity = ray.direction() * normal_;
	if(velocity == 0) return -1; // path is parallel to plane, will never intersect

	// it's really distance (not time) but this analogy works well
	double time = dist_away / velocity;
	return time;
	
	// ORIGINAL CODE
	// double b = normal_ * ( ray.origin() + ((normal_ * distance_).invert()));
	// double a = ray.direction() * normal_;
	// return -1*b/a;
		
}

#endif
