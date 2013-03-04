#pragma once

#include <vector>
#include <sstream>
#include "../globals.h"

class Logic;
struct Structure
{
	Type type;
	int rectangle;
	int from, to; // Only needed for road construction
	float food_contained;
	float money_needed, money_supplied;
	float production_time;

	float population, population_needed;

	float hunger;

	Structure( int _rectangle, Type _type, int _from = 0, int _to = 0 );
	operator std::string ();
	void Update( Logic& l, float delta_time, int& i );
};

