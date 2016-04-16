#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Vect.h"

class Camera
{
public:
	Camera();
	Camera(Vect a0, Vect b0, Vect c0, Vect d0) : position_(a0), direction_(b0), right_(c0), down_(d0) {}

	Vect position() const { return position_; }
	Vect direction() const { return direction_; }
	Vect right() const { return right_; }
	Vect down() const { return down_; }

private:
	Vect position_, direction_, right_, down_;
};

Camera::Camera()
{
	position_ = Vect(0,0,0);
	direction_ = Vect(0,0,1);
	right_ = Vect(0,0,0);
	down_ = Vect(0,0,0);
}

#endif
