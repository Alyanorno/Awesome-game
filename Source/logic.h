#pragma once

#include <vector>
#include <sstream>
#include <map>
#define GLFW_DLL
#include "..\GL\include\glew.h"
#include "..\GL\include\glfw.h"
#include "globals.h"


class Logic
{
private:
	struct Road
	{
		Road( int _rectangle, int _from, int _to );
		void Calculate();
		int rectangle, from, to;
		float length;
	};
	std::vector< Road > roads;

	struct Farm
	{
		Farm( int _rectangle );
		void Calculate();
		operator std::string ();
		int rectangle;

		float food_storage, food_contained, food_production;

		float population, population_needed;

		float hunger, moral;
	};
	std::vector< Farm > farms;

	struct City
	{
		City( int _rectangle, int _farm_rectangle );
		void Calculate();
		operator std::string ();
		int rectangle, farm_rectangle;
		float money_storage, money_contained, money_production;

		int carts;
		float cart_production_time, current_cart_production, cart_money;

		int soldiers;
		float soldier_production_time, current_soldier_production, soldier_money;

		float population, population_needed;

		float hunger, moral;
	};
	std::vector< City > cities;

	struct Structure
	{
		Structure( int _rectangle, Type _type );
		operator std::string ();
		Type type;
		int rectangle;
		float food_contained;
		float money_needed, money_supplied;
		float production_time;

		float population, population_needed;

		float hunger;
	};
	std::vector< Structure > structures;

	struct Army
	{
		Army( int _rectangle, int _from, int _soldiers, int _carts );
		void Calculate();
		operator std::string ();
		int rectangle;
		int soldiers;
		int carts;
		float storage_capacity;
		float food_stored, money_stored;
		float food_consumed, money_consumed;

		float x, y;
		float speed;
		int from, to, final_to;
		bool transporting;
		int transporting_from, transporting_to;
		bool stationary;

		float hunger, moral;
	};
	std::vector< Army > armies;

	double last_time;
	float food_per_person, population_increase;
	static float L( float _x ) { return _x < 0 ? -_x: _x; }
	int CalculatePathTo( Army& _a, int _to );
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

