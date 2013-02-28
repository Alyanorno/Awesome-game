#include "logic.h"

Logic::Road::Road( int _rectangle, int _from, int _to ) : rectangle(_rectangle), from(_from), to(_to)
{
	Calculate();
}
void Logic::Road::Calculate()
{
	Rectangle& r( rectangles[ (int)Type::Road ].v[rectangle] );
	Rectangle& to_r( rectangles[ (int)Type::Farm ].v[from] );
	Rectangle& from_r( rectangles[ (int)Type::Farm ].v[to] );

	r.x = (to_r.x + from_r.x) / 2;
	r.y = (to_r.y + from_r.y) / 2;

	length = sqrt( pow( L(to_r.x - from_r.x), 2 ) + pow( L(to_r.y - from_r.y), 2 ) );
	r.scale = length;

	r.rotation = atan( (to_r.x - from_r.x) / (to_r.y - from_r.y) ) * 180 / 3.14159;
}




// TODO: Implement
int Logic::CalculatePathTo( Army& _a, int _to )
{
	return _to; // temp

	std::vector<int> closed, open;
	std::map<int,int> map; // map of navigated nodes

	float shortest = 0; // Cost from start along best known path
	int current = _a.from;
	open.push_back( current );
	// TODO: Add variable(s) estimated total cost from start to goal througb y

	while( open.size() )
	{
		if( current == _to )
		{
			// Found path to goal
		}

		// Remove current from open
		// Add current to closed

		// find each road going from current
		std::vector<int> from_current;

		for( int i(0); i < from_current.size(); i++ )
		{
			// if what line leads to is contained in closed
			// 	continue
			Road& r( roads[ from_current[i] ] );
			bool Continue = false;
			for each( int j in closed )
				if( r.from == current ? r.to == j: r.from == j )
				{
					Continue = true;
					break;
				}
			if( Continue )
				continue;

		//	cost = cost of current + length of line
		//
		//	if what line leads to not in open || cost < total cost to what line leads to
		//		map[ what line leads to ] = current
		//		total cost of path to what line leads to = cost
		//		estimate of cost from what line leads to to goal = cost of path to what line leads to + quess of cost from what line leads to to goal
		//		if what line leads to not in open
		//			add it to open
		}
	}

	// Path not found
	_a.stationary = true;
	return _a.from;
}



void Logic::BuildCarts( int _rectangle, int _amount )
{
	for( int i(0); i < cities.size(); i++ )
		if( cities[i].rectangle == _rectangle )
			cities[i].carts += _amount;
}

void Logic::BuildSoldiers( int _rectangle, int _amount )
{
	for( int i(0); i < cities.size(); i++ )
		if( cities[i].rectangle == _rectangle )
			cities[i].soldiers += _amount;
}


void Logic::BuildRoad( int _from, int _to )
{
	// TODO: Add construction of road
	int t = rectangles[ (int)Type::Road ].insert( Rectangle( 0, 0, 0 ) );
	roads.push_back( Road( t, _from, _to ) );
}
void Logic::BuildFarm( float _x, float _y, float _scale )
{
	int t = rectangles[ (int)Type::Structure ].insert( Rectangle( _x, _y, _scale ) );
	structures.push_back( Structure( t, Type::Farm ) );
}
void Logic::BuildCity( float _x, float _y, float _scale )
{
	int t = rectangles[ (int)Type::Structure ].insert( Rectangle( _x, _y, _scale ) );
	structures.push_back( Structure( t, Type::City ) );
}
void Logic::ExpandFarm( int _rectangle, float _size )
{
	// TODO: Add construction before expanding.
	for each( Farm f in farms )
		if( f.rectangle == _rectangle )
		{
			f.Calculate();
			break;
		}
}
void Logic::ExpandCity( int _rectangle, float _size )
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
std::pair< int, float > Logic::ClosestFarm( float _x, float _y )
{
	float distance = 10000;
	int closest = -1;
	for( int i(0); i < rectangles[ (int)Type::Farm ].v.size(); i++ )
	{
		float t = Distance( _x, _y, rectangles[ (int)Type::Farm ].v[i].x, rectangles[ (int)Type::Farm ].v[i].y );
		if( t < distance )
		{
			distance = t;
			closest = i; 
		}
	}
	return std::make_pair( closest, distance );
}
std::pair< int, float > Logic::ClosestCity( float _x, float _y )
{
	float distance = 10000;
	int closest = -1;
	for( int i(0); i < rectangles[ (int)Type::City ].v.size(); i++ )
	{
		float t = Distance( _x, _y, rectangles[ (int)Type::City ].v[i].x, rectangles[ (int)Type::City ].v[i].y );
		if( t < distance )
		{
			distance = t;
			closest = i; 
		}
	}
	return std::make_pair( closest, distance );
}
std::pair< int, float > Logic::ClosestStructure( float _x, float _y )
{
	float distance = 10000;
	int closest = -1;
	for( int i(0); i < rectangles[ (int)Type::Structure ].v.size(); i++ )
	{
		float t = Distance( _x, _y, rectangles[ (int)Type::Structure ].v[i].x, rectangles[ (int)Type::Structure ].v[i].y );
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


bool Logic::OverLappingFarm( float _x, float _y, float _scale )
{
	for each( Farm f in farms )
	{
		Rectangle& r( rectangles[ (int)Type::Farm ].v[f.rectangle] );
		if( (_scale + r.scale) / 2 >= sqrt( pow( _x - r.x, 2) + pow( _y - r.y, 2 ) ) )
			return true;
	}
	return false;
}
bool Logic::OverLappingCity( float _x, float _y, float _scale )
{
	for each( City c in cities )
	{
		Rectangle& r( rectangles[ (int)Type::City ].v[c.rectangle] );
		if( (_scale + r.scale) / 2 >= sqrt( pow( _x - r.x, 2) + pow( _y - r.y, 2 ) ) )
			return true;
	}
	return false;
}


std::string Logic::GetInfo( int _rectangle, Type _t )
{
	switch( _t )
	{
		case Type::Structure:
			for each( Structure s in structures )
				if( s.rectangle == _rectangle )
					return s;
			break;
		case Type::Farm:
			for each( Farm f in farms )
				if( f.rectangle == _rectangle )
					return f;
			break;
		case Type::City:
			for each( City c in cities )
				if( c.rectangle == _rectangle )
					return c;
			break;
		case Type::Army:
			return armies[_rectangle];
			break;
	}
	return "NO INFO";
}


void Logic::ArmyTo( int _army, int _to )
{
	Army& a( armies[_army] );
	a.final_to = _to;
	a.to = CalculatePathTo( a, a.final_to );
	a.stationary = false;
	a.transporting = false;
}
void Logic::ArmyTransport( int _army, int _to )
{
	Army& a( armies[_army] );
	a.final_to = _to;
	a.to = CalculatePathTo( a, a.final_to );
	a.transporting_from = a.from;
	a.transporting_to = a.to;
	a.stationary = false;
	a.transporting = true;

	bool found = false;
	for( int j(0); j < farms.size(); j++ )
		if( farms[j].rectangle == a.from )
		{
			a.food_stored = a.storage_capacity;
			farms[j].food_contained -= a.food_stored;
			found = true;
		}
	if( !found )
		throw std::string("Invalid target for transporting food"); // TODO: Do this better
}
std::pair<float,float> Logic::ArmyPosition( int _army )
{
	return std::make_pair( armies[_army].x, armies[_army].y );
}
float Logic::ArmySize( int _army )
{
	return rectangles[ (int)Type::Army ].v[ armies[_army].rectangle ].scale;
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
bool Logic::TopLineFromEqualsTo()
{
	return lines.back().start == lines.back().end;
}
bool Logic::TopLineEqualsOtherLine()
{
	for( int i(2); i < lines.size()-1; i++ )
		if( lines[i] == lines.back() )
			return true;
	return false;
}


void Logic::Initialize()
{
	last_time = glfwGetTime();
	food_per_person = 0.01f;
	population_increase = 0.01f;
	int t = rectangles[ (int)Type::Farm ].insert( Rectangle( 0, 0, 1 ) );
	farms.push_back( Farm( t ) );
}

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
		farms[i].Update( *this, delta_time );
	for( int i(0); i < cities.size(); i++ )
		cities[i].Update( *this, delta_time );
	for( int i(0); i < structures.size(); i++ )
		structures[i].Update( *this, delta_time, i );
	for( int i(0); i < armies.size(); i++ )
		armies[i].Update( *this, delta_time, i );
}

