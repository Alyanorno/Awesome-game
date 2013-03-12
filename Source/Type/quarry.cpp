#include "quarry.h"
#include "../logic.h"

Quarry::Quarry( int _rectangle, int _point ) : rectangle(_rectangle), point(_point), food_contained(0), stone_contained(0), population(0), hunger(0), used(true)
{
	Calculate();
}
Quarry::operator std::string ()
{
	std::stringstream s;
	s << "LUMBER CAMP " << rectangle << std::endl
	  << (int)(food_contained+.5f) << "/" << (int)(food_storage+.5f) << "#" << std::endl
	  << (int)(stone_contained+.5f) << "/" << (int)(stone_storage+.5f) << "#" << std::endl
	  << "+" << (int)(stone_production+.5f) << "#/S" << std::endl
	  << (int)(population+.5f) << "/" << (int)(population_needed+.5f) << " PEOPLE" << std::endl;
	return s.str();
}

void Quarry::Calculate()
{
	float size = 3.14159 * rectangles[ (int)Type::Quarry ][rectangle].scale * rectangles[ (int)Type::Quarry ][rectangle].scale;
	food_storage = size * 2;
	stone_storage = size * 10;
	stone_production = size;

	population_needed = size * 100;
}

void Quarry::Update( Logic& l, float delta_time )
{
	if( !used )
		return;

	float efficency = population / population_needed;
	if( efficency > 1.f )
		efficency = 1.f;

	stone_contained += stone_production * delta_time * efficency;
	if( stone_contained > stone_storage )
		stone_contained = stone_storage;

	// TODO: Reduce mountain / or make it more require more effort to extract more stone

	l.PopulationCalculations( food_contained, population, hunger, delta_time );
}
