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
		Road( int _line, int _from, int _to );
		void Calculate();
		int line;
		float length;
		int from, to;
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
		Army( int _soldiers, int _carts, float _x, float _y );
		void Calculate();
		operator std::string ();
		int soldiers;
		int carts;
		float storage_capacity;
		float food_stored, money_stored;
		float food_consumed, money_consumed;

		float x, y;
		float speed;
		int from, to;
		bool stationary;
	};
	std::vector< Army > armies;

	double last_time;
public:
	void BuildCarts( int _rectangle, int _amount );
	void BuildSoldiers( int _rectangle, int _amount );

	void BuildRoad( int _line, int _from, int _to );
	void BuildFarm( int _x, int _y, int _scale, int _texture );
	void BuildCity( int _x, int _y, int _scale, int _texture );

	void ExpandFarm( int _rectangle, int _size );
	void ExpandCity( int _rectangle, int _size );

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

	std::string GetInfo( int _rectangle );
	std::string GetArmyInfo( int _x );

	void ArmyTo( int _army, int _to );

	void AddLine( float _x, float _y, float __x, float __y );
	void AddRectangle( float _x, float _y, float _scale, int _texture = 1 );

	void RemoveTopLine();
	void RemoveTopRectangle();

	void MoveTopLine( float _x, float _y );
	void MoveTopRectangle( float _x, float _y );
	void ResizeTopRectangle( float _scale );


	void Initialize();
	void Update();
};

