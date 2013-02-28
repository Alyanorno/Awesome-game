#pragma once

#include <vector>
#include <sstream>
#include <map>
#include "../globals.h"


class Logic;
struct City
{
	int rectangle, farm_rectangle;
	float money_storage, money_contained, money_production;

	int carts;
	float cart_production_time, current_cart_production, cart_money;

	int soldiers;
	float soldier_production_time, current_soldier_production, soldier_money;

	float population, population_needed;

	float hunger, moral;

	City( int _rectangle, int _farm_rectangle );
	operator std::string ();
	void Calculate();
	void Update( Logic& l, float delta_time );
};

