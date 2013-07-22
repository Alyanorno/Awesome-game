#include "structure.h"
#include "../logic.h"


Structure::Structure( Logic& l, int _rectangle, Type _type, int _point, bool _expand, int _from, int _to ) : rectangle(_rectangle), type(_type), point(_point), from(_from), to(_to), food_contained(0), build_progress(0), build_complete(0), hunger(0), used(true), expand(_expand)
{
	if( !expand )
		Calculate();
	else
		CalculateExpansion(l);
}
void Structure::Calculate()
{
	Rectangle& r( rectangles[ (int)Type::Structure ][rectangle] );
	float size = 3.14159 * r.scale * r.scale;
	switch( type )
	{
		case Type::Road:
			build_complete = 0.5 * r.scale_x;
			break;
		case Type::Farm:
			build_complete = 5 * size;
			break;
		case Type::City:
			build_complete = 5 * size;
			break;
		case Type::Quarry:
			build_complete = 5 * size;
			break;
		case Type::LumberCamp:
			build_complete = 5 * size;
			break;
		default:
			throw std::string( "Invalid type for construction: " + std::to_string( (int)type ) );
	}

	population_needed = size * 100;
	population = population_needed; // temp
	food_contained = population * 100; // temp
}
void Structure::CalculateExpansion( Logic& l )
{
	Rectangle& s_r( rectangles[ (int)Type::Structure ][rectangle] );
	float s_size = 3.14159 * s_r.scale * s_r.scale;
	float size;

	if( type == Type::Farm )
	{
		Farm& f( l.GetFarmByPoint( point ) );
		Rectangle& r( rectangles[ (int)Type::Farm ][ f.rectangle ] );
		size = s_size - 3.14159 * r.scale * r.scale;
		build_complete = 5 * size;
	}
	else if( type == Type::City )
	{
		City& c( l.GetCityByPoint( point ) );
		Rectangle& r( rectangles[ (int)Type::City ][ c.rectangle ] );
		size = s_size - 3.14159 * r.scale * r.scale;
		build_complete = 5 * size;
	}
	else if( type == Type::Quarry )
	{
		Quarry& q( l.GetByPoint<Quarry>( point ) );
		Rectangle& r( rectangles[ (int)Type::Quarry ][ q.rectangle ] );
		size = s_size - 3.14159 * r.scale * r.scale;
		build_complete = 5 * size;
	}
	else if( type == Type::LumberCamp )
	{
		LumberCamp& l( l.GetByPoint<LumberCamp>( point ) );
		Rectangle& r( rectangles[ (int)Type::LumberCamp ][ l.rectangle ] );
		size = s_size - 3.14159 * r.scale * r.scale;
		build_complete = 5 * size;
	}
	else
			throw std::string( "Invalid type for construction: " + std::to_string( (int)type ) );

	population_needed = size * 100;
	population = population_needed; // temp
	food_contained = population * 100; // temp
}

Structure::operator std::string()
{
	std::stringstream s;
	s << "STRUCTURE" << std::endl << (int)(build_progress+.5f) << "/" << (int)(build_complete+.5f) << " PROGRESS" << std::endl << (int)(population+.5f) << "/" << (int)(population_needed+.5f) << " PEOPLE" << std::endl;
	return s.str();
}

void Structure::Update( Logic& l, float delta_time, int i )
{
	if( !used )
		return;

	l.PopulationCalculations( food_contained, population, hunger, delta_time );

	float efficency = population / population_needed;
	if( efficency > 1.f )
		efficency = 1.f;

	production_time -= delta_time * efficency;
	build_progress += (population_needed / 100) * delta_time * efficency;

	int t;
	std::map< Type, int >& p( l.GetPoint( point ).on_point );
	Rectangle& sr( rectangles[ (int)Type::Structure ][rectangle] );
	if( build_progress > build_complete )
	{
		switch( type )
		{
			case Type::Road:
				t = rectangles[ (int)Type::Road ].insert( Rectangle( 0, 0, 0 ) );
				l.ChangeRoad( rectangles[ (int)Type::Road ][t], from, to );
				t = l.Get<Road>().insert( Road( t, from, to ) );

				l.GetByIndex<Road>( t ).length = sqrt( pow( Logic::L(l.GetPoint(to).x - l.GetPoint(from).x), 2 ) + pow( Logic::L(l.GetPoint(to).y - l.GetPoint(from).y), 2 ) );
				p.erase( Type::Structure );
				p[ Type::Road ] = t;

				// TODO: Create migration group
				// population;
				// food_contained;
				break;
			case Type::Farm:
				if( expand )
				{
					t = l.GetFarmIndex( point );
					rectangles[ (int)Type::Farm ][ l.GetFarmByIndex(t).rectangle ].scale = sr.scale;
					l.GetFarmByIndex(t).Calculate();
				}
				else
				{
					t = rectangles[ (int)Type::Farm ].insert( Rectangle( sr.x, sr.y, sr.scale ) );
					t = l.GetFarms().insert( Farm( t, point ) );
				}
				p.erase( Type::Structure );
				p[ Type::Farm ] = t;

				l.GetFarmByIndex(t).population += population;
				l.GetFarmByIndex(t).food += food_contained;
				break;
			case Type::Quarry:
				if( expand )
				{
					t = l.GetIndex<Quarry>( point );
					rectangles[ (int)Type::Quarry ][ l.GetByIndex<Quarry>(t).rectangle ].scale = sr.scale;
					l.GetByIndex<Quarry>(t).Calculate();
				}
				else
				{
					t = rectangles[ (int)Type::Quarry ].insert( Rectangle( sr.x, sr.y, sr.scale ) );
					t = l.Get<Quarry>().insert( Quarry( t, point ) );
				}
				p.erase( Type::Structure );
				p[ Type::Quarry ] = t;

				l.GetByIndex<Quarry>(t).population += population;
				l.GetByIndex<Quarry>(t).food_contained += food_contained;
				break;
			case Type::LumberCamp:
				if( expand )
				{
					t = l.GetIndex<LumberCamp>( point );
					rectangles[ (int)Type::LumberCamp ][ l.GetByIndex<LumberCamp>(t).rectangle ].scale = sr.scale;
					l.GetByIndex<LumberCamp>(t).Calculate();
				}
				else
				{
					t = rectangles[ (int)Type::LumberCamp ].insert( Rectangle( sr.x, sr.y, sr.scale ) );
					t = l.Get<LumberCamp>().insert( LumberCamp( t, point ) );
				}
				p.erase( Type::Structure );
				p[ Type::LumberCamp ] = t;

				l.GetByIndex<LumberCamp>(t).population += population;
				l.GetByIndex<LumberCamp>(t).food_contained += food_contained;
				break;
			case Type::City:
				if( expand )
				{
					t = l.GetCityIndex( point );
					rectangles[ (int)Type::City ][ l.GetCityByIndex(t).rectangle ].scale = sr.scale;
					l.GetCityByIndex(t).Calculate();
					p.erase( Type::Structure );
				}
				else
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

							l.GetCityByIndex(t).population += population;
							l.GetCityByIndex(t).food_contained += food_contained;
							break;
						}
					}
				break;
		}
		rectangles[ (int)Type::Structure ].erase( rectangle );
		l.GetStructures().erase( i );
	}
}

