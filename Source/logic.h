#pragma once

#include "globals.h"

class Logic
{
private:
public:
	void BuildRoad() {}
	void BuildFarm() {}
	void BuildCity() {}

	void ExpandFarm() {}
	void ExpandCity() {}


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

