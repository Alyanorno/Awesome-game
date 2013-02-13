#include "logic.h"


Logic::Road::Road( int _line, int _from, int _to ) : line(_line), from(_from), to(_to)
{
	Calculate();
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
void Logic::Farm::Calculate()
{
	float size = rectangles.v[rectangle].scale;
	food_storage = size * 10;
	food_production = size;
}

Logic::City::City( int _rectangle, int _farm_rectangle ) : rectangle(_rectangle), farm_rectangle(_farm_rectangle), money_contained(0), carts(0), cart_production_time(1), cart_money(1), soldiers(0), soldier_production_time(1), soldier_money(1)
{
	Calculate();
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

Logic::Army::Army( int _soldiers, int _carts, float _x, float _y ) : soldiers(_soldiers), carts(_carts), x(_x), y(_y), food_stored(0), money_stored(0), food_consumed(0.1), money_consumed(0.1), speed(0.5), stationary(true)
{
	Calculate();
}
void Logic::Army::Calculate()
{
	storage_capacity = soldiers * 1 + carts * 10;
}



void Logic::BuildCarts( int _rectangle, int _amount )
{
	for each( City c in cities )
		if( c.rectangle == _rectangle )
			c.carts += _amount;
}

void Logic::BuildSoldiers( int _rectangle, int _amount )
{
	for each( City c in cities )
		if( c.rectangle == _rectangle )
			c.soldiers += _amount;
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
	{
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
		if( c.money_contained <= 0 )
			continue;
		if( c.carts )
		{
			c.money_contained -= c.cart_production_time / time * c.cart_money;
			c.current_cart_production -= c.cart_production_time / time;
			if( c.current_cart_production <= 0 )
			{
				float x = rectangles.v[c.rectangle].x;
				float y = rectangles.v[c.rectangle].y;
				bool army_in_city = false;
				for each( Army a in armies )
					if( a.x == x && a.y == y && a.stationary )
					{
						army_in_city = true;
						a.carts += 1;
						break;
					}

				if( !army_in_city )
					armies.push_back( Army( 0, 1, x, y ) );
			}
		}
		if( c.soldiers )
		{
			c.money_contained -= c.soldier_production_time / time * c.soldier_money;
			c.current_soldier_production -= c.soldier_production_time / time;
			if( c.current_soldier_production <= 0 )
			{
				float x = rectangles.v[c.rectangle].x;
				float y = rectangles.v[c.rectangle].y;
				bool army_in_city = false;
				for each( Army a in armies )
					if( a.x == x && a.y == y )
					{
						army_in_city = true;
						a.soldiers += 1;
						break;
					}

				if( !army_in_city )
					armies.push_back( Army( 0, 1, x, y ) );
			}
		}
	}
	for( int i(0); i < structures.size(); i++ )
	{
		Structure& s = structures[i];
		if( s.money_supplied <= 0.0 )
			continue;
		s.production_time -= time;
		s.money_supplied -= s.money_per_second * time;

		Rectangle r;
		if( s.production_time <= 0 )
		{
			switch( s.type )
			{
				case road:
					// TODO: Add later when road have been changed to using texture.
					break;
				case farm:
					rectangles.v[ s.rectangle ].texture = 1; // TODO: Change
					break;
				case city:
					rectangles.v[ s.rectangle ].texture = 0; // TODO: Change
					break;
			}
			structures.erase( structures.begin() + i );
			i--;
		}
	}
	for each( Army a in armies )
	{
		// Movement
		if( !a.stationary )
		{
			Rectangle& r = rectangles.v[a.to];
			float distance = a.speed * time;
			float d_x = r.x - a.x;
			float d_y = r.y - a.y;
			if( d_y == 0 )
				d_y = 0.0001;
			float t = (d_x < 0 ? -d_x : d_x) / (d_y < 0 ? -d_y : d_y);
			float x = (r.x > a.x ? distance : -distance) * t;
			float y = (r.y > a.y ? distance : -distance) * (1-t); // Possibly wrong, needs to be tested.
			if( r.x - a.x <= x )
				a.x = r.x;
			if( r.y - a.y <= y )
				a.y = r.y;
			
			if( a.x == r.x && a.y == r.x )
			{
				a.from = a.to;
				a.stationary = true;
			}
			else
			{
				a.x += x;
				a.y += y;
			}
		}

		// TODO: If in city or on farm, take food from city or farm instead.
		a.food_stored -= a.food_consumed * time;
		a.money_stored -= a.money_consumed * time;
		if( a.food_stored <= 0 )
		{
			// TODO: Start the starving... muahahahaha.
		}
		if( a.money_stored <= 0 )
		{
			// TODO: They dont like you... MUTINY.
		}
	}
}

