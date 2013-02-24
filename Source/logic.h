#pragma once

#include <vector>
#include <sstream>
#define GLFW_DLL
#include "..\GL\include\glew.h"
#include "..\GL\include\glfw.h"
#include "globals.h"


class Logic
{
private:
	enum Type { road, farm, city };

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
		float food_storage;
		float food_contained;
		float food_production;
	};
	std::vector< Farm > farms;

	struct City
	{
		City( int _rectangle, int _farm_rectangle );
		void Calculate();
		operator std::string ();
		int rectangle, farm_rectangle;
		float money_storage;
		float money_contained;
		float money_production;
		float food_consumed;

		int carts;
		float cart_production_time;
		float current_cart_production;
		float cart_money;

		int soldiers;
		float soldier_production_time;
		float current_soldier_production;
		float soldier_money;
	};
	std::vector< City > cities;

	struct Structure
	{
		Structure( int _rectangle, Type _type );
		operator std::string ();
		Type type;
		int rectangle;
		float money_needed, money_supplied;
		float production_time;
	};
	std::vector< Structure > structures;

	struct Army
	{
		Army( int _rectangle, int _soldiers, int _carts );
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
	};
	std::vector< Army > armies;

	double last_time;
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
	std::pair< int, float > ClosestRectangle( float _x, float _y );
	std::pair< int, float > ClosestFarm( float _x, float _y );
	std::pair< int, float > ClosestCity( float _x, float _y );
	std::pair< int, float > ClosestStructure( float _x, float _y );
	std::pair< int, float > ClosestArmy( float _x, float _y );

	bool OverLappingFarm( float _x, float _y, float _scale );
	bool OverLappingCity( float _x, float _y, float _scale );

	std::string GetInfo( int _rectangle );
	std::string GetArmyInfo( int _x );

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

