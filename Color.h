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


private:
	double red_, green_, blue_, special_;
};

#endif
