#include "logic.h"



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
	_a.state = Army::Stationary;
	return _a.from;
}



void Logic::ToggleCartProduction( int _point )
{
	City& c( cities[ points[_point].on_point[ Type::City ] ] );
	c.cart.isProducing = c.cart.isProducing ? false: true;
}

void Logic::ToggleSoldierProduction( int _point )
{
	City& c( cities[ points[_point].on_point[ Type::City ] ] );
	c.soldier.isProducing = c.soldier.isProducing ? false: true;
}


template < class T > void Logic::Build( int _from, int _to ) {}
template <> void Logic::Build<Road>( int _from, int _to )
{
	if( _from == _to )
		return;

	// Check if road already exists
	auto roads = Get<Road>();
	if( roads.contains( [_from,_to]( Road& r ) -> bool
	{ 
		return (r.from == _from && r.to == _to) || (r.from == _to && r.to == _from);
	} ))
		return;

	int t = rectangles[ (int)Type::Structure ].insert( Rectangle( 0, 0, 0 ) );
	ChangeRoad( rectangles[ (int)Type::Structure][t], _from, _to );
	structures.insert( Structure( *this, t, Type::Road, 0, false, _from, _to ) );
}
template < class T > void Logic::Build( float _x, float _y, float _scale )
{
	int t = rectangles[ (int)Type::Structure ].insert( Rectangle( _x, _y, _scale ) );
	int t2 = points.insert( Point( _x, _y ) );
	points[t2].on_point[ Type::Structure ] =  structures.insert( Structure( *this, t, GetType<T>::result, t2 ) );
}
template void Logic::Build<Farm>( float _x, float _y, float _scale );
template void Logic::Build<Quarry>( float _x, float _y, float _scale );
template void Logic::Build<LumberCamp>( float _x, float _y, float _scale );
template < class T > void Logic::Build( int _point, float _scale ) {}
template <> void Logic::Build<City>( int _point, float _scale )
{
	Point& p( points[_point] );
	int t = rectangles[ (int)Type::Structure ].insert( Rectangle( p.x, p.y, _scale ) );
	p.on_point[ Type::Structure ] = structures.insert( Structure( *this, t, Type::City, _point ) );
}


void Logic::ChangeRoad( Rectangle& _rectangle, int _from, int _to )
{
	Point& from_p( points[_from] );
	Point& to_p( points[_to] );

	ChangeRoad( _rectangle, from_p, to_p );
}
void Logic::ChangeRoad( Rectangle& _rectangle, int _from, float _x, float _y )
{
	Point& from_p( points[_from] );
	Point to_p( _x, _y );
	ChangeRoad( _rectangle, from_p, to_p );
}
void Logic::ChangeRoad( Rectangle& _rectangle, Point& _from, Point& _to )
{
	Rectangle& r( _rectangle );

	r.x = (_to.x + _from.x) / 2;
	r.y = (_to.y + _from.y) / 2;

	float length = sqrt( pow( Logic::L(_to.x - _from.x), 2 ) + pow( Logic::L(_to.y - _from.y), 2 ) );
	r.scale_x = length;
	r.scale = 0.5f;

	r.rotation = atan2( (_to.x - _from.x), (_to.y - _from.y) ) * (180 / 3.14159);
}
template < class T > void Logic::Expand( int _point, float _scale )
{
	Type type( GetType<T>::result );
	Point& p( points[_point] );
	float scale = rectangles[ (int)type ][ GetBuffer<T>()[ p.on_point[ type ] ].rectangle ].scale;
	if( _scale <= scale )
		return;
	int t = rectangles[ (int)Type::Structure ].insert( Rectangle( p.x, p.y, _scale ) );
	p.on_point[ Type::Structure ] = structures.insert( Structure( *this, t, type, _point, true ) );
}
#define FOO( CLASS, ARRAY, NUMBER ) \
	template void Logic::Expand<CLASS>( int _point, float _scale );
	TYPE_TABLE
#undef FOO


float Logic::Distance( float _x, float _y, float __x, float __y )
{
	return (_x - __x) * (_x - __x) + (_y - __y) * (_y - __y);
}
std::pair< int, float > Logic::Closest( float _x, float _y )
{
	float distance = 10000;
	int closest = -1;
	for( int i(0); i < points.size(); i++ )
	{
		if( !points[i].used )
			continue;
		float t = Distance( _x, _y, points[i].x, points[i].y );
		if( t < distance )
		{
			distance = t;
			closest = i; 
		}
	}
	return std::make_pair( closest, distance );
}
std::pair< int, float > Logic::Closest( Type _type, float _x, float _y )
{
	float distance = 10000;
	int closest = -1;
	for( int i(0); i < points.size(); i++ )
	{
		if( !points[i].used )
			continue;
		if( points[i].on_point.find( _type ) == points[i].on_point.end() )
			continue;
		float t = Distance( _x, _y, points[i].x, points[i].y );
		if( t < distance )
		{
			distance = t;
			closest = i; 
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
		if( !armies[i].used )
			continue;
		float t = Distance( _x, _y, armies[i].x, armies[i].y );
		if( t < distance )
		{
			distance = t;
			closest = i; 
		}
	}
	return std::make_pair( closest, distance );
}


std::pair< bool, Type > Logic::OverLapping( float _x, float _y, float _scale )
{
#define FOO( CLASS, ARRAY, NUMBER ) \
	if( GetType<CLASS>::result != Type::Road && GetType<CLASS>::result != Type::Army && GetType<CLASS>::result != Type::Wall ) \
		for each( CLASS i in GetBuffer<CLASS>() ) \
		{ \
			Rectangle& r( rectangles[ (int)GetType<CLASS>::result ][i.rectangle] ); \
			if( r.x == _x && r.y == _y ) \
				continue; \
			if( (_scale + r.scale) / 2 >= sqrt( pow( _x - r.x, 2) + pow( _y - r.y, 2 ) ) ) \
				return std::make_pair( true, GetType<CLASS>::result ); \
		}
	TYPE_TABLE
#undef FOO
	return std::make_pair( false, Type::Size );
}
bool Logic::OverLappingCity( float _x, float _y, float _scale )
{
	for each( City i in GetBuffer<City>() )
	{
		Rectangle& r( rectangles[ (int)Type::City ][i.rectangle] );
			if( r.x == _x && r.y == _y )
				continue;
					if( (_scale + r.scale) / 2 >= sqrt( pow( _x - r.x, 2) + pow( _y - r.y, 2 ) ) )
						return true;
	}
	return false;
}


template < class T > std::string Logic::GetInfo( int _point )
{
	return GetBuffer<T>()[ (int)points[_point].on_point[ GetType<T>::result ] ];
}
#define FOO( CLASS, ARRAY, NUMBER ) \
	template std::string Logic::GetInfo<CLASS>( int _point );
	TYPE_TABLE
#undef FOO
std::string Logic::GetArmyInfo( int _army )
{
	return armies[_army];
}
void Logic::SetArmyState( int _army, Army::State _state )
{
	if( armies[_army].state != Army::Moving )
		armies[_army].state = _state;
}


void Logic::PopulationCalculations( float& _food_contained, float& _population, float& _hunger, float _delta_time )
{
	if( _hunger > 0 && _food_contained > _hunger )
	{
		_food_contained -= _hunger;
		_hunger = 0;
	}
	_food_contained -= food_per_person * _population * _delta_time;
	if( _food_contained < 0 )
	{
		_hunger -= _food_contained;
		_food_contained = 0;
	}
	float limit = _population / 10;
	if( _hunger > limit )
	{
		if( _population > 200 )
			_population *= 0.9;
		else
			_population -= 10;
		if( _population < 1 )
			_population = 0;
		limit = _population / 10;
		_hunger = limit - limit / 10;
	}

	_population += _population * population_increase * _delta_time;
}

void Logic::ArmyTo( int _army, int _to )
{
	Army& a( armies[_army] );
	a.final_to = _to;
	a.to = CalculatePathTo( a, a.final_to );
	a.state = Army::Moving;
	a.transporting = Resource::Nothing;
}
void Logic::ArmyTransport( int _army, int _to, Resource _transporting )
{
	Army& a( armies[_army] );
	a.final_to = _to;
	a.to = CalculatePathTo( a, a.final_to );
	a.transporting_from = a.from;
	a.transporting_to = a.to;
	a.state = Army::Moving;
	a.transporting = _transporting;

	a.food_stored = a.storage_capacity;
	GetCityByPoint( a.from ).food_contained -= a.food_stored;
}
std::pair<float,float> Logic::ArmyPosition( int _army )
{
	return std::make_pair( armies[_army].x, armies[_army].y );
}
float Logic::ArmySize( int _army )
{
	return rectangles[ (int)Type::Army ][ armies[_army].rectangle ].scale;
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
	population_increase = 0.001f;
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
	for( int i(0); i < quarries.size(); i++ )
		quarries[i].Update( *this, delta_time );
	for( int i(0); i < lumber_camps.size(); i++ )
		lumber_camps[i].Update( *this, delta_time );
	for( int i(0); i < structures.size(); i++ )
		structures[i].Update( *this, delta_time, i );
	for( int i(0); i < armies.size(); i++ )
		armies[i].Update( *this, delta_time, i );
}

