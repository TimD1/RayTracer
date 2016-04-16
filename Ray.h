#ifndef _RAY_H_
#define _RAY_H_

#include "Vect.h"

class Ray
{
public:
	Ray();
	Ray(Vect a0, Vect b0) : start_(a0), direction_(b0) {}

	Vect start() const { return start_; }
	Vect direction() const { return direction_; }

private:
	Vect start_, direction_;
};

Ray::Ray()
{
	start_ = Vect(0,0,0);
	direction_ = Vect(1,0,0);
}

#endif
