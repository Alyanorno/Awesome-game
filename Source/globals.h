#pragma once

#include <vector>

enum class Textures { Farm = 1, City, Structure, Army };

template <class T>
struct buffer
{
	void push_back( T _t )
	{
		for( int i(0); i < v.size(); i++ )
			if( v[i].used == false )
			{
				v[i] = _t;
				return;
			}
		v.push_back( _t );
	}
	void erase( int _i )
	{
		v[_i].used = false;
	}
	std::vector<T> v;
};

struct Rectangle
{
	Rectangle() {}
	Rectangle( float _x, float _y, float _scale, int _texture )
		: x(_x), y(_y), scale(_scale), texture(_texture), used(true) {}
	float x, y, scale;
	int texture;
	bool used;
};
extern buffer< Rectangle > rectangles;

struct Line
{
	Line( float x, float y, float to_x, float to_y )
		: start( x, y ), end( to_x, to_y ) {}
	bool operator == ( Line& l ) { return start == l.start && end == l.end; }
	struct Coords
	{
		Coords( float _x, float _y ) : x(_x), y(_y), z(0) {}
		bool operator == ( Coords& c ) { return x == c.x && y == c.y; }
		float x, y, z;
	} start, end;
};
extern std::vector< Line > lines;

