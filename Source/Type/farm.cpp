#include "farm.h"
#include "../logic.h"

Farm::Farm( int _rectangle ) : rectangle(_rectangle), food_contained(0), hunger(0)
{
	Calculate();
}
Farm::operator std::string ()
{
	std::stringstream s;
	s << "FARM " << rectangle << std::endl << (int)(food_contained+.5f) << "/" << (int)(food_storage+.5f) << "#" << std::endl << "+" << (int)(food_production+.5f) << "#/S" << std::endl << (int)(population+.5f) << "/" << (int)(population_needed+.5f) << " PEOPLE" << std::endl;
	return s.str();
}

void Farm::Calculate()
{
	float size = 3.14159 * rectangles[ (int)Type::Farm ].v[rectangle].scale * rectangles[ (int)Type::Farm ].v[rectangle].scale;
	food_storage = size * 1000;
	food_production = size * 10;

	population_needed = size * 100;
	population = population_needed; // temp
}

void Farm::Update( Logic& l, float delta_time )
{
	float efficency = population / population_needed;
	if( efficency > 1.f )
		efficency = 1.f;

	food_contained += food_production * delta_time * efficency;
	if( hunger > 0 )
	{
		food_contained -= hunger;
		hunger = 0;
	}
	food_contained -= l.food_per_person * population * delta_time;
	if( food_contained > food_storage )
		food_contained = food_storage;

	population += population * l.population_increase * delta_time;
}

