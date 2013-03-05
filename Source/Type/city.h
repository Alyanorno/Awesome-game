#pragma once

#include <vector>
#include <sstream>
#include <map>
#include "../globals.h"


class Logic;
struct City
{
	int rectangle;
	int point;
	float money_storage, money_contained, money_production;

	bool producing_carts;
	float cart_production_time, current_cart_production, cart_money;

	bool producing_soldiers;
	float soldier_production_time, current_soldier_production, soldier_money;

	float population, population_needed;

	float hunger;

	bool used;

	City( int _rectangle , int _point );
	operator std::string ();
	void Calculate();
	void Update( Logic& l, float delta_time );
};

