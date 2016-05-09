#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "Color.h"
#include "Vect.h"
#include "Object.h"

class Triangle : public Object
{
public:
	Triangle();
	Triangle(Vect A, Vect B, Vect C, Color color0);

	virtual Vect tri_normal() const { return normal_; }
	virtual double distance() const { return normal_ * A_; }	//dist to origin
	virtual Vect normal(const Vect & pt) const { return normal_; } 
	virtual Color color() const { return color_; }
	virtual double find_intersection(const Ray & ray) const; //dist from camera along ray

private:
	Vect A_, B_, C_;
	Vect normal_;
	double distance_;
	Color color_;
};

Triangle::Triangle()
{
	A_ = Vect(1, 0, 0);
	B_ = Vect(0, 1, 0);
	C_ = Vect(0, 0, 1);
	color_ = Color(0.5,0.5,0.5,0);
}

Triangle::Triangle(Vect A, Vect B, Vect C, Color color0) : A_(A), B_(B), C_(C)
{
	color_ = color0;
	normal_ = ((C_-A_)^(B_-A_)).normalize(); 
	distance_ = distance();
}

double Triangle::find_intersection(const Ray & ray) const
{
	// find vector from ray's start to point on plane "above" (closest to) origin
	Vect ray_to_plane = ray.start().invert() + (normal_ * distance_);

	// find orthogonal distance to plane 
	double ortho_dist = normal_ * ray_to_plane;

	// find orthogonal distance ray travels per unit movement
	double frac_ortho = normal_ * ray.direction();
	if (frac_ortho == 0) return -1; // ray never intersects if parallel
	
	//find point on plane
	Vect P = ray.start() + ray.direction() * (ortho_dist / frac_ortho);

	//check if this point is outside the triangle
	if( ((C_-A_)^(P-A_))*normal_ < 0 ||  ((B_-C_)^(P-C_))*normal_ < 0 
			|| ((A_-B_)^(P-B_))*normal_ < 0 )
	{
		//point is outside triangle
		return -1;
	}
	//point is inside triangle, return total distance travelled
	return ortho_dist / frac_ortho;
}

#endif
