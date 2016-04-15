#ifndef _VECT_H_
#define _VECT_H_
#include <cmath>
#include <iostream>

class Vect
{
public:
	Vect() : Vect(0, 0, 0) {};
	Vect(double x0, double y0, double z0) : x_(x0), y_(y0), z_(z0) {}

	double x() const { return x_; }
	double y() const { return y_; }
	double z() const { return z_; }
	double magnitude() const { return std::sqrt(x_*x_ + y_*y_ + z_*z_); }

	Vect normalize() const;
	Vect invert() const { return Vect(-x_, -y_, -z_); }

	double operator*(const Vect & B) const 	//dot product
	{ return x_*B.x_ + y_*B.y_ + z_*B.z_; }
	Vect operator^(const Vect & B) const	//cross product
	{ return Vect(y_*B.z_-z_*B.y_, x_*B.z_-z_*B.x_, x_*B.y_-y_*B.x_); }
	Vect operator+(const Vect & B) const	//addition
	{ return Vect(x_+B.x_, y_+B.y_, z_+B.z_); }
	Vect operator-(const Vect & B) const	//subtraction
	{ return Vect(x_-B.x_, y_-B.y_, z_-B.z_); }
	friend inline std::ostream & operator<<(std::ostream & out, const Vect & vec);
	Vect operator*(const double m) const	//scalar multiplication
	{ return Vect(x_*m, y_*m, z_*m); }

private:
	double x_, y_, z_;
};

Vect Vect::normalize() const
{
	return Vect(x_/magnitude(), y_/magnitude(), z_/magnitude());
}

inline std::ostream & operator<<(std::ostream & out, const Vect & vec)
{
	out << '(' << vec.x() << ", " << vec.y() << ", " << vec.z() << ')';
	return out;
}

#endif
