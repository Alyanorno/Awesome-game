#include "structure.h"
#include "../logic.h"


Structure::Structure( int _rectangle, Type _type ) : rectangle(_rectangle), type(_type), money_supplied(0), hunger(0)
{
	float size = 3.14159 * rectangles[ (int)Type::Structure ].v[rectangle].scale * rectangles[ (int)Type::Structure ].v[rectangle].scale;
	switch( type )
	{
		case Type::Road:
			// Doesnt exist right now
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
}

Structure::operator std::string()
{
	std::stringstream s;
	s << "STRUCTURE" << std::endl << (int)(money_supplied+.5f) << "/" << (int)(money_needed+.5f) << "$" << std::endl << (int)(production_time+.5f) << " PRODUCTION TIME" << std::endl << (int)(population+.5f) << "/" << (int)(population_needed+.5f) << " PEOPLE" << std::endl;
	return s.str();
}

void Structure::Update( Logic& l, float delta_time, int& i )
{
	if( money_supplied <= -0.001 )
		return;

	float efficency = population / population_needed;
	if( efficency > 1.f )
		efficency = 1.f;

	production_time -= delta_time * efficency;
	money_supplied -= (money_needed / production_time) * delta_time * efficency;
	money_needed -= (money_needed / production_time) * delta_time * efficency;

	if( hunger > 0 )
	{
		food_contained -= hunger;
		hunger = 0;
	}
	food_contained -= l.food_per_person * population * delta_time;


	int t;
	Rectangle r;
	Rectangle& sr( rectangles[ (int)Type::Structure ].v[rectangle] );
	if( production_time <= 0.1 )
	{
		switch( type )
		{
			case Type::Road:
				// TODO: Add later when road have been changed to using texture.
				break;
			case Type::Farm:
				t = rectangles[ (int)Type::Farm ].insert( Rectangle( sr.x, sr.y, sr.scale ) );
				l.farms.push_back( Farm( t ) );
				break;
			case Type::City:
				for each( Farm f in l.farms )
				{
					Rectangle& fr( rectangles[ (int)Type::Farm ].v[f.rectangle] );
					if( fr.x == sr.x && fr.y == fr.y )
					{
						t = rectangles[ (int)Type::City ].insert( Rectangle( sr.x, sr.y, sr.scale ) );
						l.cities.push_back( City( t, f.rectangle ) );
						break;
					}
				}
				break;
		}
		rectangles[ (int)Type::Structure ].erase( rectangle );
		l.structures.erase( l.structures.begin() + i );
		i--;
	}
	population += population * l.population_increase * delta_time;
}
