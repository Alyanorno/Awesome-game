#include "structure.h"
#include "../logic.h"


Structure::Structure( int _rectangle, Type _type, int _point, int _from, int _to ) : rectangle(_rectangle), type(_type), point(_point), from(_from), to(_to), food_contained(0), money_supplied(0), hunger(0), used(true)
{
	Rectangle& r( rectangles[ (int)Type::Structure ][rectangle] );
	float size = 3.14159 * r.scale * r.scale;
	switch( type )
	{
		case Type::Road:
			money_needed = 1 * r.scale_x;
			production_time = 0.1 * r.scale_x;
			break;
		case Type::Farm:
			money_needed = 100 * size;
			production_time = 1 * size;
			break;
		case Type::City:
			money_needed = 100 * size;
			production_time = 1 * size;
			break;
		default:
			throw std::string( "Invalid type for construction: " + std::to_string( (int)_type ) );
	}

	money_supplied = money_needed; // temp

	population_needed = size * 100;
	population = population_needed; // temp
	food_contained = population * 1000; // temp
}

Structure::operator std::string()
{
	std::stringstream s;
	s << "STRUCTURE" << std::endl << (int)(money_supplied+.5f) << "/" << (int)(money_needed+.5f) << "$" << std::endl << (int)(production_time+.5f) << " PRODUCTION TIME" << std::endl << (int)(population+.5f) << "/" << (int)(population_needed+.5f) << " PEOPLE" << std::endl;
	return s.str();
}

void Structure::Update( Logic& l, float delta_time, int i )
{
	if( !used )
		return;

	l.PopulationCalculations( food_contained, population, hunger, delta_time );

	if( money_supplied <= -0.001 )
		return;

	float efficency = population / population_needed;
	if( efficency > 1.f )
		efficency = 1.f;

	production_time -= delta_time * efficency;
	money_supplied -= (money_needed / production_time) * delta_time * efficency;
	money_needed -= (money_needed / production_time) * delta_time * efficency;

	int t;
	std::map< Type, int >& p( l.GetPoint( point ).on_point );
	Rectangle r;
	Rectangle& sr( rectangles[ (int)Type::Structure ][rectangle] );
	if( production_time <= 0.1 )
	{
		switch( type )
		{
			case Type::Road:
				t = rectangles[ (int)Type::Road ].insert( Rectangle( 0, 0, 0 ) );
				l.ChangeRoad( rectangles[ (int)Type::Road ][t], from, to );
				t = l.GetRoads().insert( Road( t, from, to ) );
				l.GetRoad( t ).length = sqrt( pow( Logic::L(l.GetPoint(to).x - l.GetPoint(from).x), 2 ) + pow( Logic::L(l.GetPoint(to).y - l.GetPoint(from).y), 2 ) );
				p.erase( Type::Structure );
				p[ Type::Road ] = t;
				break;
			case Type::Farm:
				t = rectangles[ (int)Type::Farm ].insert( Rectangle( sr.x, sr.y, sr.scale ) );
				t = l.GetFarms().insert( Farm( t, point ) );
				p.erase( Type::Structure );
				p[ Type::Farm ] = t;
				break;
			case Type::City:
				for each( Farm f in l.GetFarms() )
				{
					if( !f.used )
						continue;
					Rectangle& fr( rectangles[ (int)Type::Farm ][f.rectangle] );
					if( fr.x == sr.x && fr.y == fr.y )
					{
						t = rectangles[ (int)Type::City ].insert( Rectangle( sr.x, sr.y, sr.scale ) );
						t = l.GetCities().insert( City( t, point ) );
						p.erase( Type::Structure );
						p[ Type::City ] = t;
						break;
					}
				}
				break;
		}
		rectangles[ (int)Type::Structure ].erase( rectangle );
		l.GetStructures().erase( i );
	}
}

