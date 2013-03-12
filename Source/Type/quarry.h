#pragma once

#include <vector>
#include <sstream>
#include "../globals.h"

class Logic;
struct Quarry
{
	int rectangle;
	int point;

	float food_contained, food_storage;
	float stone_production, stone_contained, stone_storage;
	float population, population_needed;
	float hunger;

	bool used;

	Quarry( int _rectangle, int _point );
	operator std::string ();
	void Calculate();
	void Update( Logic& l, float delta_time );
};

