#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Color.h"
#include "Vect.h"

class Light
{
public:
	Light();
	Light(Vect vect0, Color color0) : position_(vect0), color_(color0) {}

	Vect vect() const { return position_; }
	Color color() const { return color_; }

private:
	Vect position_;
	Color color_;
};

Light::Light()
{
	position_ = Vect(0,0,0);
	color_ = Color(1,1,1,0);
}

#endif
