#pragma once

#include <vector>
#include <sstream>
#include "../globals.h"

class Logic;
struct LumberCamp
{
	typedef LumberCamp type_of;
	int rectangle;
	int point;

	float food_contained, food_storage;
	float wood_production, wood_contained, wood_storage;
	float population, population_needed;
	float hunger;

	bool used;

	LumberCamp( int _rectangle, int _point );
	operator std::string ();
	void Calculate();
	void Update( Logic& l, float delta_time );
};

