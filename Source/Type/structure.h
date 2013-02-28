#pragma once

#include <vector>
#include <sstream>
#include "../globals.h"

class Logic;
struct Structure
{
	Type type;
	int rectangle;
	float food_contained;
	float money_needed, money_supplied;
	float production_time;

	float population, population_needed;

	float hunger;

	Structure( int _rectangle, Type _type );
	operator std::string ();
	void Update( Logic& l, float delta_time, int& i );
};

