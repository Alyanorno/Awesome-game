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
	float size = 3.14 * rectangles.v[rectangle].scale * rectangles.v[rectangle].scale;
	food_storage = size * 100;
	food_production = size;
}
Logic::Farm::operator std::string()
{
	std::stringstream s;
	s << "FARM " << rectangle << std::endl << (int)(food_contained+.5f) << "/" << (int)(food_storage+.5f) << "#" << std::endl << "+" << (int)(food_production+.5f) << "#/S" << std::endl;
	return s.str();
}

Logic::City::City( int _rectangle, int _farm_rectangle ) : rectangle(_rectangle), farm_rectangle(_farm_rectangle), money_contained(0), carts(0), cart_production_time(1), current_cart_production( cart_production_time ), cart_money(1), soldiers(0), soldier_production_time(1), current_soldier_production( soldier_production_time ), soldier_money(1)
{
	Calculate();
}
void Logic::City::Calculate()
{
	float size = 3.14 * rectangles.v[rectangle].scale * rectangles.v[rectangle].scale;
	money_storage = size * 10;
	money_production = size * 2;
	food_consumed = size * 2;

	//temp
	soldiers = 10;
}
Logic::City::operator std::string()
{
	std::stringstream s;
	s << "CITY " << rectangle << std::endl << (int)(money_contained+.5f) << "/" << (int)(money_storage+.5f) << "$" << std::endl << "+" << (int)(money_production+.5f) << "$/S" << std::endl << "-" << (int)(food_consumed+.5f) << "#/S" << std::endl << soldiers << " SOLDIERS IN PRODUCTION" << std::endl << carts << " CARTS IN PRODUCTION" << std::endl;
	return s.str();
}

Logic::Structure::Structure( int _rectangle, Type _type ) : rectangle(_rectangle), type(_type), money_supplied(0)
{
	float size = 3.14 * rectangles.v[rectangle].scale * rectangles.v[rectangle].scale;
	switch( type )
	{
		case road:
			// Doesnt exist right now
			break;
		case farm:
			money_needed = 100 * size;
			production_time = 1 * size;
			break;
		case city:
			money_needed = 100 * size;
			production_time = 1 * size;
			break;
	}

	// temp
	money_supplied = money_needed;
}
Logic::Structure::operator std::string()
{
	std::stringstream s;
	s << "STRUCTURE" << std::endl << (int)(money_supplied+.5f) << "/" << (int)(money_needed+.5f) << "$" << std::endl << (int)(production_time+.5f) << " PRODUCTION TIME" << std::endl;
	return s.str();
}

Logic::Army::Army( int _rectangle, int _soldiers, int _carts ) : rectangle(_rectangle), soldiers(_soldiers), carts(_carts), food_stored(0), money_stored(0), food_consumed(0.1), money_consumed(0.1), speed(1), stationary(true)
{
	x = rectangles.v[rectangle].x;
	y = rectangles.v[rectangle].y;
	Calculate();
}
void Logic::Army::Calculate()
{
	storage_capacity = soldiers * 1 + carts * 10;
	rectangles.v[rectangle].x = x;
	rectangles.v[rectangle].y = y;
	rectangles.v[rectangle].scale = sqrt( (soldiers + carts) / 3.14 );
}
Logic::Army::operator std::string()
{
	std::stringstream s;
	s << "ARMY" << std::endl << (int)(soldiers+.5f) << " SOLDIERS " << (int)(carts+.5f) << " CARTS" << std::endl << (int)(food_stored+.5f) << "# " << (int)(money_stored+.5f) << "$" << std::endl;
	return s.str();
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
void Logic::BuildFarm( float _x, float _y, int _scale )
{
	rectangles.push_back( Rectangle( _x, _y, _scale, (int)Textures::Structure ) );
	structures.push_back( Structure( rectangles.v.size()-1, farm ) );
}
void Logic::BuildCity( float _x, float _y, int _scale )
{
	rectangles.push_back( Rectangle( _x, _y, _scale, (int)Textures::Structure ) );
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


float Logic::Distance( float _x, float _y, float __x, float __y )
{
	return (_x - __x) * (_x - __x) + (_y - __y) * (_y - __y);
}
std::pair< int, float > Logic::ClosestRectangle( float _x, float _y )
{
	float distance = 1000;
	int closest = -1;
	for( int i(0); i < rectangles.v.size(); i++ )
	{
		float t = Distance( _x, _y, rectangles.v[i].x, rectangles.v[i].y );
		if( t < distance )
		{
			distance = t;
			closest = i; 
		}
	}
	return std::make_pair( closest, distance );
}
std::pair< int, float > Logic::ClosestFarm( float _x, float _y )
{
	float distance = 10000;
	int closest = -1;
	for( int i(0); i < rectangles.v.size(); i++ )
	{
		float t = Distance( _x, _y, rectangles.v[i].x, rectangles.v[i].y );
		if( t < distance )
		{
			bool farm = false;
			for each( Farm f in farms )
				if( f.rectangle == i )
				{
					farm = true;
					break;
				}
			if( farm )
			{
				distance = t;
				closest = i; 
			}
		}
	}
	return std::make_pair( closest, distance );
}
std::pair< int, float > Logic::ClosestCity( float _x, float _y )
{
	float distance = 10000;
	int closest = -1;
	for( int i(0); i < rectangles.v.size(); i++ )
	{
		float t = Distance( _x, _y, rectangles.v[i].x, rectangles.v[i].y );
		if( t < distance )
		{
			bool city = false;
			for each( City c in cities )
				if( c.rectangle == i )
				{
					city = true;
					break;
				}
			if( city )
			{
				distance = t;
				closest = i; 
			}
		}
	}
	return std::make_pair( closest, distance );
}
std::pair< int, float > Logic::ClosestStructure( float _x, float _y )
{
	float distance = 10000;
	int closest = -1;
	for( int i(0); i < rectangles.v.size(); i++ )
	{
		float t = Distance( _x, _y, rectangles.v[i].x, rectangles.v[i].y );
		if( t < distance )
		{
			bool structure = false;
			for each( Structure s in structures )
				if( s.rectangle == i )
				{
					structure = true;
					break;
				}
			if( structure )
			{
				distance = t;
				closest = i; 
			}
		}
	}
	return std::make_pair( closest, distance );
}
std::pair< int, float > Logic::ClosestArmy( float _x, float _y )
{
	float distance = 10000;
	int closest = -1;
	for( int i(0); i < armies.size(); i++ )
	{
		float t = Distance( _x, _y, armies[i].x, armies[i].y );
		if( t < distance )
		{
			distance = t;
			closest = i; 
		}
	}
	return std::make_pair( closest, distance );
}

std::string Logic::GetInfo( int _rectangle )
{
	for each( Structure s in structures )
		if( s.rectangle == _rectangle )
			return s;
	for each( Farm f in farms )
		if( f.rectangle == _rectangle )
			return f;
	for each( City c in cities )
		if( c.rectangle == _rectangle )
			return c;
	return "NO INFO";
}
std::string Logic::GetArmyInfo( int _x )
{
	return armies[_x];
}


void Logic::ArmyTo( int _army, int _to )
{
	armies[_army].to = _to;
	armies[_army].stationary = false;
}


void Logic::AddLine( float _x, float _y, float __x, float __y )
{
	lines.push_back( Line( _x, _y, __x, __y ) );
}
void Logic::RemoveTopLine()
{
	if( lines.size() <= 2 ) return; lines.pop_back();
}
void Logic::MoveTopLine( float _x, float _y )
{
	if( lines.size() > 2 ) lines.back().end = Line::Coords( _x, _y );
}


void Logic::Initialize()
{
	last_time = glfwGetTime();
	rectangles.push_back( Rectangle( 0, 0, 1, (int)Textures::Farm ) );
	farms.push_back( Farm( rectangles.v.size()-1 ) );
}

float L( float _x ) { return _x < 0 ? -_x: _x; }
void Logic::Update()
{
	double time = glfwGetTime();
	if( last_time + 0.01 > time )
	{
		return;
	}
	float delta_time = time - last_time;
	last_time = time;

	for( int i(0); i < farms.size(); i++ )
	{
		Farm& f( farms[i] );
		f.food_contained += f.food_production * delta_time;
		if( f.food_contained > f.food_storage )
			f.food_contained = f.food_storage;
	}
	for( int i(0); i < cities.size(); i++ )
	{
		City& c( cities[i] );
		for( int j(0); j < farms.size(); j++ )
		{
			Farm& f( farms[j] );
			if( f.rectangle == c.farm_rectangle )
			{
				if( f.food_contained <= 0 )
					break;
				f.food_contained -= c.food_consumed * delta_time;
				
				c.money_contained += c.money_production * delta_time;
				if( c.money_contained > c.money_storage )
					c.money_contained = c.money_storage;
				break;
			}
		}
		if( c.money_contained <= 0 )
			continue;
		if( c.carts )
		{
			c.money_contained -= c.cart_money * delta_time;
			c.current_cart_production -= delta_time;
			if( c.current_cart_production <= 0 )
			{
				float x = rectangles.v[c.rectangle].x;
				float y = rectangles.v[c.rectangle].y;
				bool army_in_city = false;
				for( int j(0); j < armies.size(); j++ )
				{
					if( armies[j].x == x && armies[j].y == y )
					{
						army_in_city = true;
						armies[j].carts++;
						armies[j].Calculate();
						break;
					}
				}

				if( !army_in_city )
				{
					rectangles.push_back( Rectangle( x, y, 1, (int)Textures::Army ) );
					armies.push_back( Army( rectangles.v.size()-1, 0, 1 ) );
				}
				c.current_cart_production = c.soldier_production_time;
				c.carts--;
			}
		}
		if( c.soldiers )
		{
			c.money_contained -= c.soldier_money * delta_time;
			c.current_soldier_production -= delta_time;
			if( c.current_soldier_production <= 0 )
			{
				float x = rectangles.v[c.rectangle].x;
				float y = rectangles.v[c.rectangle].y;
				bool army_in_city = false;
				for( int j(0); j < armies.size(); j++ )
				{
					if( armies[j].x == x && armies[j].y == y )
					{
						army_in_city = true;
						armies[j].soldiers++;
						armies[j].Calculate();
						break;
					}
				}

				if( !army_in_city )
				{
					rectangles.push_back( Rectangle( x, y, 1, (int)Textures::Army ) );
					armies.push_back( Army( rectangles.v.size()-1, 1, 0 ) );
				}
				c.current_soldier_production = c.soldier_production_time;
				c.soldiers--;
			}
		}
	}
	for( int i(0); i < structures.size(); i++ )
	{
		Structure& s( structures[i] );
		if( s.money_supplied <= -0.001 )
			continue;
		s.production_time -= delta_time;
		s.money_supplied -= (s.money_needed / s.production_time) * delta_time;
		s.money_needed -= (s.money_needed / s.production_time) * delta_time;

		Rectangle r;
		if( s.production_time <= 0.1 )
		{
			switch( s.type )
			{
				case road:
					// TODO: Add later when road have been changed to using texture.
					break;
				case farm:
					rectangles.v[s.rectangle].texture = (int)Textures::Farm;
					farms.push_back( Farm( s.rectangle ) );
					break;
				case city:
					for each( Farm f in farms )
					{
						Rectangle& fr( rectangles.v[f.rectangle] );
						Rectangle& sr( rectangles.v[s.rectangle] );
						if( fr.x == sr.x && fr.y == fr.y )
						{
							rectangles.v[s.rectangle].texture = (int)Textures::City;
							cities.push_back( City( s.rectangle, f.rectangle ) );
							break;
						}
					}
					break;
			}
			structures.erase( structures.begin() + i );
			i--;
		}
	}
	for( int i(0); i < armies.size(); i++ )
	{
		Army& a( armies[i] );
		// Movement
		if( !a.stationary )
		{
			Rectangle& r = rectangles.v[a.to];
			float distance = a.speed * delta_time;
			float d_x = r.x - a.x;
			float d_y = r.y - a.y;
			if( d_y == 0 )
				d_y = 0.0001;
			float t = L(d_x) / ( L(d_x) + L(d_y) );
			float x = (r.x > a.x ? distance : -distance) * t;
			float y = (r.y > a.y ? distance : -distance) * (1-t);
			if( L(r.x - a.x) <= L(x) )
				a.x = r.x;
			if( L(r.y - a.y) <= L(y) )
				a.y = r.y;

			if( a.x == r.x && a.y == r.y )
			{
				for( int j(0); j < armies.size(); j++ )
					if( i != j && a.x == armies[j].x && a.y == armies[j].y )
					{
						a.soldiers += armies[j].soldiers;
						a.carts += armies[j].carts;
						rectangles.erase( armies[j].rectangle );
						armies.erase( armies.begin() + j );
						break;
					}
				a.from = a.to;
				a.stationary = true;
			}
			else
			{
				a.x += x;
				a.y += y;
			}
			a.Calculate();
		}

		// TODO: If in city or on farm, take food from city or farm instead.
		if( a.food_stored <= 0 )
		{
			// TODO: Start the starving... muahahahaha.
		}
		else
			a.food_stored -= a.food_consumed * delta_time;
			
		if( a.money_stored <= 0 )
		{
			// TODO: They dont like you... MUTINY.
		}
		else
			a.money_stored -= a.money_consumed * delta_time;
	}
}

