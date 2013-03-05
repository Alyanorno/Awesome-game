#include "farm.h"
#include "../logic.h"

Farm::Farm( int _rectangle, int _point ) : rectangle(_rectangle), point(_point), food_contained(0), hunger(0), used(true)
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
	float size = 3.14159 * rectangles[ (int)Type::Farm ][rectangle].scale * rectangles[ (int)Type::Farm ][rectangle].scale;
	food_storage = size * 1000;
	food_production = size * 10;

	population_needed = size * 100;
	population = population_needed; // temp
}

void Farm::Update( Logic& l, float delta_time )
{
	if( !used )
		return;

	float efficency = population / population_needed;
	if( efficency > 1.f )
		efficency = 1.f;
	food_contained += food_production * delta_time * efficency;

	l.PopulationCalculations( food_contained, population, hunger, delta_time );
}

