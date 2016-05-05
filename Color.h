#ifndef _COLOR_H_
#define _COLOR_H_

#include <ostream>

class Color
{
public:
	Color() : Color(0.5,0.5,0.5,0) {} //gray is default
	Color(const Color & x);
	Color(double r0, double g0, double b0, double s0)
		: red_(r0), blue_(b0), green_(g0), special_(s0) {}
	
	double r() const { return red_; }
	double g() const { return green_; }
	double b() const { return blue_; }
	double s() const { return special_; }

	double set_red(double new_red) { red_ = new_red; return red_; }
	double set_green(double new_green) { green_ = new_green; return green_; }
	double set_blue(double new_blue) { blue_ = new_blue; return blue_; }
	double set_special(double new_special) { special_ = new_special; return special_; }

	double brightness() { return (red_ + green_ + blue_) / 3; }
	Color clip();
	
	Color operator*(double x) { return Color(red_*x, green_*x, blue_*x, special_*x); }
	Color operator/(double x) { return Color(red_/x, green_/x, blue_/x, special_/x); }
	Color operator*(const Color & x)
		{ return Color(red_*x.r(), green_*x.g(), blue_*x.b(), special_*x.s()); }
	Color operator+(const Color & x) 
		{ return Color(red_+x.r(), green_+x.g(), blue_+x.b(), special_+x.s()); }
	Color operator+=(const Color & rhs);
	Color operator=(const Color & rhs);
	friend std::ostream & operator<<(std::ostream & out, const Color & color);

private:
	double red_, green_, blue_, special_;
};

inline Color::Color(const Color & color)
{
	red_ = color.red_;
	green_ = color.green_;
	blue_ = color.blue_;
	special_ = color.special_;
}
	
inline std::ostream & operator<<(std::ostream & out, const Color & color)
{
	out << "(" << color.red_ << ", " << color.green_ << ", " << color.blue_
		<< ", " << color.special_ << ")";
	return out;
}

inline Color Color::operator+=(const Color & rhs)
{
	red_ = red_ + rhs.red_;
	green_ = green_ + rhs.green_;
	blue_ = blue_ + rhs.blue_;
	special_ = special_ + rhs.special_;
	return *this;
}

inline Color Color::operator=(const Color & rhs)
{
	red_ = rhs.red_;
	green_ = rhs.green_;
	blue_ = rhs.blue_;
	special_ = rhs.special_;
	return *this;
}

Color Color::clip()
{
	double all_light = red_ + green_ + blue_;
	double excess_light = all_light - 3;
	
	// check if there is too much light
	if (excess_light > 0)
	{
		// proportionally redistribute excess light
		red_ = red_ + excess_light * (red_ / all_light);
		green_ = green_ + excess_light * (green_ / all_light);
		blue_ = blue_ + excess_light * (blue_ / all_light);
	}
	
	// clip if there's still excess light
	if(red_ > 1) red_ = 1;
	if(green_ > 1) green_ = 1;
	if(blue_ > 1) blue_ = 1;
	
	return *this;
}

#endif
