#pragma once

#include <vector>
#include <sstream>
#include <map>
#define GLFW_DLL
#include "..\GL\include\glew.h"
#include "..\GL\include\glfw.h"
#include "globals.h"
#include "Type/farm.h"
#include "Type/city.h"
#include "Type/structure.h"
#include "Type/army.h"
#include "Type/quarry.h"
#include "Type/lumber_camp.h"

struct Point
{
	Point( float _x, float _y ) : x(_x), y(_y), used(true) {}
	void On( Type _type, int _i ) { on_point[_type] = _i; }
	float x, y;
	std::map< Type, int > on_point;
	bool used;
};
struct Road
{
	typedef Road type_of;
	Road( int _rectangle, int _from, int _to ) : rectangle(_rectangle), from(_from), to(_to) {}
	int rectangle;
	int from, to;
	float length;
	bool used;
};
struct Wall
{
	Wall( int _rectangle, int _point ) : rectangle(_rectangle), point(_point) {}
	int rectangle, point;
	bool used;
};
class Logic
{
private:
	buffer< Road > roads;
	buffer< Farm > farms;
	buffer< City > cities;
	buffer< Quarry > quarries;
	buffer< LumberCamp > lumber_camps;
	buffer< Structure > structures;
	buffer< Army > armies;
	buffer< Wall > walls;

	buffer< Point > points;

	double last_time;



	template < class T > struct GetType
		{ static const Type result = Type::Size; };
	template <> struct GetType <Road>
		{ static const Type result = Type::Road; };
	template <> struct GetType <Farm>
		{ static const Type result = Type::Farm; };
	template <> struct GetType <City>
		{ static const Type result = Type::City; };
	template <> struct GetType <Quarry>
		{ static const Type result = Type::Quarry; };
	template <> struct GetType <LumberCamp>
		{ static const Type result = Type::LumberCamp; };
	template <> struct GetType <Army>
		{ static const Type result = Type::Army; };
	template <> struct GetType <Structure>
		{ static const Type result = Type::Structure; };
	template <> struct GetType <Wall>
		{ static const Type result = Type::Wall; };

	struct Nothing {};
	template < int T > struct GetClass
		{ typedef Nothing result; };
	template <> struct GetClass <0>
		{ typedef Road result; };
	template <> struct GetClass <1>
		{ typedef Farm result; };
	template <> struct GetClass <2>
		{ typedef City result; };
	template <> struct GetClass <3>
		{ typedef Quarry result; };
	template <> struct GetClass <4>
		{ typedef LumberCamp result; };
	template <> struct GetClass <5>
		{ typedef Army result; };
	template <> struct GetClass <6>
		{ typedef Structure result; };
	template <> struct GetClass <7>
		{ typedef Wall result; };

	template < int T > buffer< typename GetClass<T>::result >& GetBufferForType()
	{
		switch( (Type)T )
		{
			case Type::Road:
				return roads;
			case Type::Farm:
				return farms;
			case Type::City:
				return cities;
			case Type::Quarry:
				return quarries;
			case Type::LumberCamp:
				return lumber_camps;
			case Type::Structure:
				return structures;
			case Type::Army:
				return armies;
			case Type::Wall:
				return walls;
			case Type::Nothing
				throw std::string("Can't get type of nothing");
		}
	}
	template < class T > buffer< T >& GetBuffer()
		{ throw std::string("Invalid buffer type"); }
	template <> buffer< Road >& GetBuffer()
		{ return roads; }
	template <> buffer< Farm >& GetBuffer()
		{ return farms; }
	template <> buffer< City >& GetBuffer()
		{ return cities; }
	template <> buffer< Quarry >& GetBuffer()
		{ return quarries; }
	template <> buffer< LumberCamp >& GetBuffer()
		{ return lumber_camps; }
	template <> buffer< Structure >& GetBuffer()
		{ return structures; }
	template <> buffer< Army >& GetBuffer()
		{ return armies; }
	template <> buffer< Wall >& GetBuffer()
		{ return walls; }
public:
	float food_per_person, population_increase;
	static float L( float _x ) { return _x < 0 ? -_x: _x; }

	template < class T > buffer<T>& Get()
		{ return GetBuffer<T>(); }

	template < class T > T& GetByIndex( int _index )
		{ return GetBuffer<T>()[_index]; }

	template < class T > T& GetByPoint( int _point )
		{ return GetBuffer<T>()[ GetPoint(_point).on_point[ GetType<T>::result ] ]; }

	template < class T > int GetIndex( int _point )
	{
		std::map< Type, int >& p( GetPoint(_point).on_point );
		if( p.find( GetType<T>::result ) == p.end() )
			return -1;
		else
			return p[ GetType<T>::result ];
	}
	template <> int GetIndex<Army>( int _rectangle )
	{
		for( int i(0); i < armies.size(); i++ )
			if( armies[i].used && armies[i].rectangle == _rectangle )
				return i;
		return -1;
	}

	template < class T > void Remove( int _point )
	{
		Type& t( GetType<T>::result );
		std::map< Type, int >& p( GetPoint(_point).on_point );
		if( p.find( t ) == p.end() )
			return;
		rectangles[ (int)t ].erase( p[ t ] );
		GetBuffer<T>().erase( p[ t ] );
		p.erase( t );
		if( p.empty() )
			points.erase(_point);
	}

	Army& GetArmyByRectangle( int _rectangle )
	{
		for( int i(0); i < armies.size(); i++ )
			if( armies[i].used && armies[i].rectangle == _rectangle )
				return armies[i];
		throw std::string("Could not find Army" );
	}





//
//
// START OF TO BE REMOVED
//
// 
	buffer< Army >& GetArmies() { return armies; }
	Army& GetArmyByIndex( int _i )
	{
		return armies[_i];
	}
	int GetArmyIndex( int _rectangle )
	{
		for( int i(0); i < armies.size(); i++ )
			if( armies[i].used && armies[i].rectangle == _rectangle )
				return i;
		return -1;
	}

#define GET( NAME, PLURAL, LOWER_CASE ) \
	buffer< NAME >& Get##PLURAL() { return LOWER_CASE; } \
	NAME& Get##NAME##ByIndex( int _index ) \
	{ \
		return LOWER_CASE[_index]; \
	} \
	NAME& Get##NAME##ByPoint( int _point ) \
	{ \
		return LOWER_CASE[ GetPoint( _point ).on_point[ Type::NAME ] ]; \
	} \
	int Get##NAME##Index( int _point ) \
	{ \
		std::map< Type, int >& p( GetPoint(_point).on_point ); \
		if( p.find( Type::NAME ) == p.end() ) \
			return -1; \
		else \
			return p[ Type::NAME ]; \
	} \
	void Remove##NAME( int _point ) \
	{ \
		std::map< Type, int >& p( GetPoint(_point).on_point ); \
		if( p.find( Type::NAME ) == p.end() ) \
			return; \
		rectangles[ (int)Type::NAME ].erase( p[ Type::NAME ] ); \
		LOWER_CASE.erase( p[ Type::NAME ] ); \
		p.erase( Type::NAME ); \
		if( p.empty() ) \
			points.erase(_point); \
	}
	GET( Road, Roads, roads )
	GET( Farm, Farms, farms )
	GET( City, Cities, cities )
	GET( Structure, Structures, structures )
#undef GEa

	void ExpandFarm( int _point, float _scale );
	void ExpandCity( int _point, float _scale );

	bool OverLappingFarm( float _x, float _y, float _scale );
	bool OverLappingCity( float _x, float _y, float _scale );
//
//
// END OF TO BE REMOVED
//
//






	buffer< Point >& GetPoints() { return points; }
	Point& GetPoint( int _i ) { return points[_i]; }
	int AddPointOn( float _x, float _y )
	{
		points.push_back( Point( _x, _y ) );
		return points.size()-1;
	}
	int FindPointOn( float _x, float _y )
	{
		for( int i(0); i < points.size(); i++ )
			if( points[i].x == _x && points[i].y == _y )
				return i;
		return -1;
	}

	int CalculatePathTo( Army& _a, int _to );

	// Interface to Input
	void ToggleCartProduction( int _rectangle );
	void ToggleSoldierProduction( int _rectangle );


	template < class T > void Build( int _from, int _to );
	template < class T > void Build( float _x, float _y, float _scale );
	template < class T > void Build( int _point, float _scale );

	void ChangeRoad( Rectangle& _rectangle, int _from, int _to, int _point = -1 );
	template < class T> void Expand( int _point, float _scale );

	float Distance( float _x, float _y, float __x, float __y );
	std::pair< int, float > Closest( float _x, float _y );
	std::pair< int, float > Closest( Type _type, float _x, float _y );
	std::pair< int, float > ClosestArmy( float _x, float _y );

	template < class T > bool OverLapping( float _x, float _y, float _scale );

	template < class T > std::string GetInfo( int _point );
	std::string GetArmyInfo( int _army );
	void SetArmyState( int _army, Army::State _state );

	void PopulationCalculations( float& _food_contained, float& _population, float& _hunger, float _delta_time );

	void ArmyTo( int _army, int _to );
	void ArmyTransport( int _army, int _to, Resource _transporting );
	std::pair<float,float> ArmyPosition( int _army );
	float ArmySize( int _army );

	void AddLine( float _x, float _y, float __x, float __y );
	void RemoveTopLine();
	void MoveTopLine( float _x, float _y );
	bool Logic::TopLineFromEqualsTo();
	bool Logic::TopLineEqualsOtherLine();

	void Initialize();
	void Update();
};

