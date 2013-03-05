#pragma once

#include <vector>
#include <sstream>
#include "../globals.h"

class Logic;
struct Army
{
	int rectangle;
	int soldiers;
	int carts;
	int people;
	float storage_capacity;
	float food_stored, money_stored;
	float food_consumed, money_consumed;

	float x, y;
	float speed;
	int from, to, final_to;
	Resource transporting;
	int transporting_from, transporting_to;
	bool stationary;

	float hunger;

	bool used;

	void Move( Logic& l, float delta_time, int& i );
	void Transport( Logic& l, float delta_time );

	Army( int _rectangle, int _from, int _soldiers, int _carts );
	operator std::string ();
	void Calculate();
	void Update( Logic& l, float delta_time, int i );
};

