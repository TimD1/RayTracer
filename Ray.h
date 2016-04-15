#ifndef _RAY_H_
#define _RAY_H_
#include <cmath>

#include "Vect.h"

class Ray
{
public:
	Ray();
	Ray(Vect a0, Vect b0) : origin_(a0), direction_(b0) {}

	Vect origin() const { return origin_; }
	Vect direction() const { return direction_; }

private:
	Vect origin_, direction_;
};

Ray::Ray()
{
	origin_ = Vect(0,0,0);
	direction_ = Vect(1,0,0);
}

#endif
