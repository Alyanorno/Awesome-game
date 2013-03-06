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
	float food_storage, food_contained;
	float money_storage, money_contained, money_production;

	struct Producing
	{
		Producing( float _time, float _resource ) : isProducing(false), time(_time), current_time(time), resource(_resource) {}
		bool isProducing;
		float time, current_time;
		float resource;
	};
	Producing cart;
	Producing soldier;

	float population, population_needed;
	float hunger;
	float size;

	bool used;

	City( int _rectangle , int _point );
	operator std::string ();
	void Calculate();
	void Update( Logic& l, float delta_time );
};

