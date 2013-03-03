#pragma once

#include <vector>
#include <sstream>
#include "../globals.h"

class Logic;
struct Farm
{
	int rectangle;

	float food_storage, food_contained, food_production;
	float population, population_needed;
	float hunger;

	Farm( int _rectangle );
	operator std::string ();
	void Calculate();
	void Update( Logic& l, float delta_time );
};

