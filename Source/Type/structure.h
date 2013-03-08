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

	float production_time;

	float build_progress, build_complete;

	float population, population_needed;

	float hunger;
	
	bool used, expand;

	Structure( Logic& l, int _rectangle, Type _type, int _point, bool _expand = false, int _from = -1, int _to = -1 );
	void Calculate();
	void CalculateExpansion( Logic& l );
	operator std::string ();
	void Update( Logic& l, float delta_time, int i );
};

