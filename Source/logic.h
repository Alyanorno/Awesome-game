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

struct Road
{
	Road( int _rectangle, int _from, int _to );
	int rectangle, from, to;
	float length;
};
class Logic
{
private:
	std::vector< Road > roads;
	std::vector< Farm > farms;
	std::vector< City > cities;
	std::vector< Structure > structures;
	std::vector< Army > armies;

	double last_time;
public:
	float food_per_person, population_increase;
	static float L( float _x ) { return _x < 0 ? -_x: _x; }
#define GET( NAME, PLURAL, LOWER_CASE ) \
	std::vector< NAME >& Get##PLURAL() { return LOWER_CASE; } \
	NAME& Get##NAME( int _rectangle ) \
	{ \
		for( int i(0); i < LOWER_CASE.size(); i++ ) \
			if( LOWER_CASE[i].rectangle == _rectangle ) \
				return LOWER_CASE[i]; \
		throw std::string("Could not find NAME" ); \
	} \
	int Get##NAME##Index( int _rectangle ) \
	{ \
		for( int i(0); i < LOWER_CASE.size(); i++ ) \
			if( LOWER_CASE[i].rectangle == _rectangle ) \
				return i; \
		return -1; \
	}
	GET( Road, Roads, roads )
	GET( Farm, Farms, farms )
	GET( City, Cities, cities )
	GET( Structure, Structures, structures )
	GET( Army, Armies, armies )
#undef GET

	int CalculatePathTo( Army& _a, int _to );

	// Interface to Input
	void ToggleCartProduction( int _rectangle );
	void ToggleSoldierProduction( int _rectangle );

	void BuildRoad( int _from, int _to );
	void BuildFarm( float _x, float _y, float _scale );
	void BuildCity( float _x, float _y, float _scale );

	void ChangeRoad( Rectangle& _rectangle, int _from, int _to );
	void ExpandFarm( int _rectangle, float _size );
	void ExpandCity( int _rectangle, float _size );

	void DestroyRoad( int _line );
	void DestroyFarm( int _rectangle );
	void DestroyCity( int _rectangle );
	void RemoveStructure( int _rectangle );

	float Distance( float _x, float _y, float __x, float __y );
	std::pair< int, float > ClosestFarm( float _x, float _y );
	std::pair< int, float > ClosestCity( float _x, float _y );
	std::pair< int, float > ClosestStructure( float _x, float _y );
	std::pair< int, float > ClosestArmy( float _x, float _y );

	bool OverLappingFarm( float _x, float _y, float _scale );
	bool OverLappingCity( float _x, float _y, float _scale );

	std::string GetInfo( int _rectangle, Type _t );

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

