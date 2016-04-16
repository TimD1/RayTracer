#ifndef _COLOR_H_
#define _COLOR_H_

class Color
{
public:
	Color() : Color(0.5,0.5,0.5,0) {}
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
	Color operator*(Color x)
		{ return Color(red_*x.r(), green_*x.g(), blue_*x.b(), special_*x.s()); }
	Color operator+(Color x) 
		{ return Color(red_+x.r(), green_+x.g(), blue_+x.b(), special_+x.s()); }

private:
	double red_, green_, blue_, special_;
};

Color Color::clip()
{
	double all_light = red_ + green_ + blue_;
	double excess_light = all_light - 3;
	if (excess_light > 0)
	{
		red_ = red_ + excess_light * (red_ / all_light);
		green_ = green_ + excess_light * (green_ / all_light);
		blue_ = blue_ + excess_light * (blue_ / all_light);
	}
	
	if(red_ > 1) red_ = 1;
	if(green_ > 1) green_ = 1;
	if(blue_ > 1) blue_ = 1;
	
	if(red_ < 0) red_ = 0;
	if(green_ < 0) green_ = 0;
	if(blue_ < 0) blue_ = 0;

	return Color(red_, green_,blue_, special_);
}

#endif
