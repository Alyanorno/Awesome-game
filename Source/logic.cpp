#include "logic.h"


Logic::Road::Road( int _line, int _from, int _to ) : line(_line), from(_from), to(_to)
{
	Calculate();
}
Logic::Road::~Road()
{
	// TODO: Not possible right now, must be changed to be using rectangles.
	//lines.erase( lines.begin() + line );
}
void Logic::Road::Calculate()
{
	float x = lines[line].start.x - lines[line].end.x;
	float y = lines[line].start.y - lines[line].end.y;
	length = x * x + y * y;
}

Logic::Farm::Farm( int _rectangle ) : rectangle(_rectangle), food_contained(0)
{
	Calculate();
}
Logic::Farm::~Farm()
{
	rectangles.erase( rectangle );
}
void Logic::Farm::Calculate()
{
	float size = rectangles.v[rectangle].scale;
	food_storage = size * 10;
	food_production = size;
}

Logic::City::City( int _rectangle, int _farm_rectangle ) : rectangle(_rectangle), farm_rectangle(_farm_rectangle), money_contained(0)
{
	Calculate();
}
Logic::City::~City()
{
	rectangles.erase( rectangle );
}
void Logic::City::Calculate()
{
	float size = rectangles.v[rectangle].scale;
	money_storage = size * 10;
	money_production = size;
	food_consumed = size;
}

Logic::Structure::Structure( int _rectangle, Type _type ) : rectangle(_rectangle), type(_type), money_supplied(0)
{
	int size;
	switch( type )
	{
		case road:
			// Doesnt exists right now
			break;
		case farm:
			size = rectangles.v[rectangle].scale;
			money_needed = 100 * size;
			production_time = 100 * size;
			money_per_second = 1;
			break;
		case city:
			size = rectangles.v[rectangle].scale;
			money_needed = 100 * size;
			production_time = 100 * size;
			money_per_second = 1;
			break;
	}
}
Logic::Structure::~Structure()
{
	rectangles.erase( rectangle );
}


void Logic::BuildRoad( int _line, int _from, int _to )
{
	roads.push_back( Road( _line, _from, _to ) );
}
void Logic::BuildFarm( int _x, int _y, int _scale, int _texture )
{
	rectangles.push_back( Rectangle( _x, _y, _scale, _texture ) );
	structures.push_back( Structure( rectangles.v.size()-1, farm ) );
}
void Logic::BuildCity( int _x, int _y, int _scale, int _texture )
{
	rectangles.push_back( Rectangle( _x, _y, _scale, _texture ) );
	structures.push_back( Structure( rectangles.v.size()-1, city ) );
}
void Logic::ExpandFarm( int _rectangle, int _size )
{
	// TODO: Add construction before expanding.
	for each( Farm f in farms )
		if( f.rectangle == _rectangle )
		{
			f.Calculate();
			break;
		}
}
void Logic::ExpandCity( int _rectangle, int _size )
{
	// TODO: Add construction before expanding.
	for each( City c in cities )
		if( c.rectangle == _rectangle )
		{
			c.Calculate();
			break;
		}
}
void Logic::DestroyRoad( int _line )
{
	// Not possible at the moment
}
void Logic::DestroyFarm( int _rectangle )
{
	int i = 0;
	for( ; i < farms.size(); i++ )
		if( farms[i].rectangle == _rectangle )
			break;
	farms.erase( farms.begin() + i );
}
void Logic::DestroyCity( int _rectangle )
{
	int i = 0;
	for( ; i < cities.size(); i++ )
		if( cities[i].rectangle == _rectangle )
			break;
	cities.erase( cities.begin() + i );
}
void Logic::RemoveStructure( int _rectangle )
{
	int i = 0;
	for( ; i < structures.size(); i++ )
		if( structures[i].rectangle == _rectangle )
			break;
	structures.erase( structures.begin() + i );
}


std::pair< float, float > Logic::RoadLocation( float _x, float _y )
{
	return std::make_pair( _x, _y );
}
std::pair< float, float > Logic::FarmLocation( float _x, float _y )
{
	return std::make_pair( _x, _y );
}
std::pair< float, float > Logic::CityLocation( float _x, float _y )
{
	return std::make_pair( _x, _y );
}


void Logic::AddLine( float _x, float _y, float __x, float __y )
{
	lines.push_back( Line( _x, _y, __x, __y ) );
}
void Logic::AddRectangle( float _x, float _y, float _scale, int _texture )
{ 
	rectangles.push_back( Rectangle( _x, _y, _scale, _texture ) );
}
void Logic::RemoveTopLine()
{
	if( lines.size() <= 2 ) return; lines.pop_back();
}
void Logic::RemoveTopRectangle()
{
	rectangles.v.pop_back();
}
void Logic::MoveTopLine( float _x, float _y )
{
	if( lines.size() > 2 ) lines.back().end = Line::Coords( _x, _y );
}
void Logic::MoveTopRectangle( float _x, float _y )
{
	rectangles.v.back().x = _x; rectangles.v.back().y = _y;
}
void Logic::ResizeTopRectangle( float _scale )
{
	rectangles.v.back().scale = _scale;
}


void Logic::Initialize()
{
	last_time = glfwGetTime();
	rectangles.push_back( Rectangle( 0, 0, 1, 1 ) );
}

void Logic::Update()
{
	double time = glfwGetTime();
	if( last_time + 0.01 < time )
	{
		return;
	}
	last_time = time;

	for each ( Farm f in farms )
	{
		f.food_contained += f.food_production * time;
		if( f.food_contained > f.food_storage )
			f.food_contained = f.food_storage;
	}
	for each ( City c in cities )
		for each ( Farm f in farms )
			if( f.rectangle == c.farm_rectangle )
			{
				if( f.food_storage <= 0 )
					break;
				f.food_storage -= c.food_consumed * time;
				
				c.money_contained += c.money_production * time;
				if( c.money_contained > c.money_storage )
					c.money_contained = c.money_storage;
				break;
			}
	for( int i(0); i < structures.size(); i++ )
	{
		Structure* s = &structures[i];
		if( s->money_supplied <= 0 )
			continue;
		s->production_time -= time;
		s->money_supplied -= s->money_per_second * time;

		Rectangle r;
		if( s->production_time <= 0 )
		{
			switch( s->type )
			{
				case road:
					// TODO: Add later when road have been changed to using texture.
					break;
				case farm:
					r = rectangles.v[ s->rectangle ];
					r.texture = 1; // TODO: Change
					rectangles.push_back( r );
					break;
				case city:
					r = rectangles.v[ s->rectangle ];
					r.texture = 0; // TODO: Change
					rectangles.push_back( r );
					break;
			}
			rectangles.erase( s->rectangle );
			structures.erase( structures.begin() + i );
			i--;
		}
	}
}

