#pragma once

#include <vector>

struct Rectangle
{
	Rectangle( float _x, float _y, float _scale, int _texture )
		: x(_x), y(_y), scale(_scale), texture(_texture) {}
	float x, y, scale;
	int texture;
};
extern std::vector< Rectangle > rectangles;

struct Line
{
	Line( float x, float y, float to_x, float to_y )
		: start( x, y ), end( to_x, to_y ) {}
	struct Coords
	{
		Coords( float _x, float _y ) : x(_x), y(_y), z(0) {}
		float x, y, z;
	} start, end;
};
extern std::vector< Line > lines;

