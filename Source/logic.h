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


class Logic
{
friend void Farm::Update( Logic& l, float delta_time );
friend void City::Update( Logic& l, float delta_time );
friend void Structure::Update( Logic& l, float delta_time, int& i );
friend void Army::Update( Logic& l, float delta_time, int& i );
private:
	struct Road
	{
		Road( int _rectangle, int _from, int _to );
		void Calculate();
		int rectangle, from, to;
		float length;
	};
	std::vector< Road > roads;

	std::vector< Farm > farms;

	std::vector< City > cities;

	std::vector< Structure > structures;

	std::vector< Army > armies;

	double last_time;
	float food_per_person, population_increase;
	int CalculatePathTo( Army& _a, int _to );
	static float L( float _x ) { return _x < 0 ? -_x: _x; }
public:
	void BuildCarts( int _rectangle, int _amount );
	void BuildSoldiers( int _rectangle, int _amount );

	void BuildRoad( int _from, int _to );
	void BuildFarm( float _x, float _y, float _scale );
	void BuildCity( float _x, float _y, float _scale );

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

	void ArmyTo( int _army, int _to );
	void ArmyTransport( int _army, int _to );
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

