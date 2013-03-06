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
	Road( int _rectangle, int _from, int _to ) : rectangle(_rectangle), from(_from), to(_to) {}
	int rectangle;
	int from, to;
	float length;
	bool used;
};
class Logic
{
private:
	buffer< Road > roads;
	buffer< Farm > farms;
	buffer< City > cities;
	buffer< Structure > structures;
	buffer< Army > armies;

	buffer< Point > points;

	double last_time;
public:
	float food_per_person, population_increase;
	static float L( float _x ) { return _x < 0 ? -_x: _x; }
#define GET( NAME, PLURAL, LOWER_CASE ) \
	buffer< NAME >& Get##PLURAL() { return LOWER_CASE; } \
	NAME& Get##NAME( int _point ) \
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
	}
	GET( Road, Roads, roads )
	GET( Farm, Farms, farms )
	GET( City, Cities, cities )
	GET( Structure, Structures, structures )
#undef GET
	buffer< Army >& GetArmies() { return armies; }
	Army& GetArmy( int _rectangle )
	{
		for( int i(0); i < armies.size(); i++ )
			if( armies[i].used && armies[i].rectangle == _rectangle )
				return armies[i];
		throw std::string("Could not find Army" );
	}
	int GetArmyIndex( int _rectangle )
	{
		for( int i(0); i < armies.size(); i++ )
			if( armies[i].used && armies[i].rectangle == _rectangle )
				return i;
		return -1;
	}

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

	void BuildRoad( int _from, int _to );
	void BuildFarm( float _x, float _y, float _scale );
	void BuildCity( int _point, float _scale );

	void ChangeRoad( Rectangle& _rectangle, int _from, int _to, int _point = -1 );
	void ExpandFarm( int _rectangle, float _size );
	void ExpandCity( int _rectangle, float _size );

	void DestroyRoad( int _line );
	void DestroyFarm( int _rectangle );
	void DestroyCity( int _rectangle );
	void RemoveStructure( int _rectangle );

	float Distance( float _x, float _y, float __x, float __y );
	std::pair< int, float > Closest( float _x, float _y );
	std::pair< int, float > Closest( Type _type, float _x, float _y );
	std::pair< int, float > ClosestArmy( float _x, float _y );

	bool OverLappingFarm( float _x, float _y, float _scale );
	bool OverLappingCity( float _x, float _y, float _scale );

	std::string GetInfo( int _point, Type _t );
	std::string GetArmyInfo( int _army );

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

