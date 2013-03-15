#pragma once

#include <vector>
#include <sstream>
#include "../globals.h"

class Logic;
struct Army
{
	typedef Army type_of;
	int rectangle;
	int soldiers;
	int carts;
	float people, people_max;
	float storage_capacity;
	float food_stored, money_stored;
	float food_consumed, money_consumed;

	float x, y;
	float speed;
	int from, to, final_to;
	Resource transporting;
	int transporting_from, transporting_to;
	enum State { Stationary, Moving, CollectFood, CollectPeople, KillPeople, DestroyFarm, DestroyCity, DestroyStructure, Fighting } state;

	float hunger;

	bool used;

	void Move( Logic& l, float delta_time, int& i );
	void Transport( Logic& l, float delta_time );

	Army( int _rectangle, int _from, int _soldiers, int _carts );
	operator std::string ();
	void Calculate();
	void Update( Logic& l, float delta_time, int i );
};

