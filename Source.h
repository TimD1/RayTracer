#ifndef _SOURCE_H_
#define _SOURCE_H_

#include "Vect.h"
#include "Color.h"

class Source
{
public:
	Source() {}
	virtual Vect light_pos() { return Vect(0,0,0); }
	virtual Color color() { return Color(1,1,1,0); }

private:
	//nothing yet
};

#endif
