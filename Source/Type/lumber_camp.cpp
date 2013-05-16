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

	l.PopulationCalculations( food_contained, population, hunger, delta_time );

	if( wood_contained >= wood_storage )
	{
		wood_contained = wood_storage;
		return;
	}

	// Reduce forests
	auto& v( height_map.square_contained );
	auto i = std::find( v.begin(), v.end(), Resource::Wood );
	auto closest = i;
	auto& r( rectangles[ (int)Type::LumberCamp ][rectangle] );
	auto size = r.scale;
	auto closest_length = size;
	while(true)
	{
		if( i == v.end() )
		{
			if( closest_length != size )
			{
				auto& resource( height_map.square_amount[ closest - v.begin() ] );
				wood_contained += wood_production * delta_time * efficency;
				resource -= wood_production * delta_time * efficency;
				if( resource <= 0 )
					height_map.Remove( closest - v.begin() );
			}
			break;
		}
		auto s = sqrt( pow( r.x - height_map.PosX( i - v.begin() ), 2 ) + pow( r.y - height_map.PosY( i - v.begin() ), 2 ) );
		if( s < closest_length )
		{
			closest_length = s;
			closest = i;
		}
		i = std::find( ++i, v.end(), Resource::Wood );
	}
}

