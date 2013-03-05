#pragma once

#include <vector>
#include <sstream>
#include "../globals.h"

class Logic;
struct Farm
{
	int rectangle;
	int point;

	float food_storage, food_contained, food_production;
	float population, population_needed;
	float hunger;

	bool used;

	Farm( int _rectangle, int _point );
	operator std::string ();
	void Calculate();
	void Update( Logic& l, float delta_time );
};

