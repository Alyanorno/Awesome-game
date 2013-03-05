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
	int point;
	float food_contained;
	float money_needed, money_supplied;
	float production_time;

	float population, population_needed;

	float hunger;
	
	bool used;

	Structure( int _rectangle, Type _type, int _point, int _from = -1, int _to = -1 );
	operator std::string ();
	void Update( Logic& l, float delta_time, int i );
};

