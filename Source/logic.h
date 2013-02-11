#pragma once

#include <vector>
#include "globals.h"


class Logic
{
private:
	enum Type { road, farm, city };

	struct Road
	{
		Road( int _line, int _from, int _to );
		~Road();
		void Calculate();
		int line;
		float length;
		int from, to;
	};
	std::vector< Road > roads;

	struct Farm
	{
		Farm( int _rectangle );
		~Farm();
		void Calculate();
		int rectangle;
		float food_storage;
		float food_contained;
		float food_production;
	};
	std::vector< Farm > farms;

	struct City
	{
		City( int _rectangle );
		~City();
		void Calculate();
		int rectangle;
		float money_storage;
		float money_contained;
		float money_production;
	};
	std::vector< City > cities;

	struct Structure
	{
		Structure( int _rectangle, Type _type );
		~Structure();
		Type type;
		int rectangle;
		float money_needed, money_supplied;
		int production_time;
		float money_per_time_cost;
	};
	std::vector< Structure > structures;
public:
	void BuildRoad( int _line, int _from, int _to );
	void BuildFarm( int _rectangle );
	void BuildCity( int _rectangle );

	void ExpandFarm( int _rectangle, int _size );
	void ExpandCity( int _rectangle, int _size );

	void DestroyRoad( int _line );
	void DestroyFarm( int _rectangle );
	void DestroyCity( int _rectangle );
	void RemoveStructure( int _rectangle );

	std::pair< float, float > RoadLocation( float _x, float _y );
	std::pair< float, float > FarmLocation( float _x, float _y );
	std::pair< float, float > CityLocation( float _x, float _y );

	void AddLine( float _x, float _y, float __x, float __y );
	void AddRectangle( float _x, float _y, float _scale, int _texture = 1 );

	void RemoveLine( int _i );
	void RemoveRectangle( int _i );

	void RemoveTopLine();
	void RemoveTopRectangle();

	void ResizeRectangle( int _i, float _scale );

	void MoveTopLine( float _x, float _y );
	void MoveTopRectangle( float _x, float _y );
	void ResizeTopRectangle( float _scale );


	void Initialize();
	void Update();
};

