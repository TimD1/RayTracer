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

	virtual Vect normal_plane() const { return normal_; }			//orientation
	virtual double distance() const { return distance_; }	//absolute position from origin
	virtual Vect normal(const Vect & pt) const { return normal_; } 	//unnecessary?
	virtual Color color() const { return color_; }
	virtual double find_intersection(const Ray & ray) const; //dist from camera along ray vec

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

double Plane::find_intersection(const Ray & ray) const
{
	// find vector from ray's start to point on plane "above" (closest to) origin
	Vect ray_to_plane = ray.start().invert() + (normal_ * distance_);

	// find orthogonal distance to plane 
	double ortho_dist = normal_ * ray_to_plane;

	// find orthogonal distance ray travels per unit movement
	double frac_ortho = normal_ * ray.direction();
	if (frac_ortho == 0) return -1; // ray never intersects if parallel
	
	// return total distance travelled
	return ortho_dist / frac_ortho;
}

#endif
