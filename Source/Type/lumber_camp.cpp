#include "lumber_camp.h"
#include "../logic.h"

LumberCamp::LumberCamp( int _rectangle, int _point ) : rectangle(_rectangle), point(_point), food_contained(0), wood_contained(0), population(0), hunger(0), used(true)
{
	Calculate();
}
LumberCamp::operator std::string ()
{
	std::stringstream s;
	s << "LUMBER CAMP " << rectangle << std::endl
	  << (int)(food_contained+.5f) << "/" << (int)(food_storage+.5f) << "#" << std::endl
	  << (int)(wood_contained+.5f) << "/" << (int)(wood_storage+.5f) << "#" << std::endl
	  << "+" << (int)(wood_production+.5f) << "#/S" << std::endl
	  << (int)(population+.5f) << "/" << (int)(population_needed+.5f) << " PEOPLE" << std::endl;
	return s.str();
}

void LumberCamp::Calculate()
{
	float size = 3.14159 * rectangles[ (int)Type::LumberCamp ][rectangle].scale * rectangles[ (int)Type::LumberCamp ][rectangle].scale;
	food_storage = size * 2;
	wood_storage = size * 10;
	wood_production = size;

	population_needed = size * 100;
}

void LumberCamp::Update( Logic& l, float delta_time )
{
	if( !used )
		return;

	float efficency = population / population_needed;
	if( efficency > 1.f )
		efficency = 1.f;

	wood_contained += wood_production * delta_time * efficency;
	if( wood_contained > wood_storage )
		wood_contained = wood_storage;

	// TODO: Reduce forests

	l.PopulationCalculations( food_contained, population, hunger, delta_time );
}

