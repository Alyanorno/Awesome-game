#pragma once

#include <vector>

enum class Type { Road, Farm, City, Structure, Army, Size };

template <class T>
struct buffer
{
	int insert( T _t )
	{
		for( int i(0); i < v.size(); i++ )
			if( v[i].used == false )
			{
				v[i] = _t;
				return i;
			}
		v.push_back( _t );
		return v.size()-1;
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
	Rectangle( float _x, float _y, float _scale, float _rotation = 0 )
		: x(_x), y(_y), scale(_scale), rotation(_rotation), used(true) {}
	float x, y, scale, rotation;
	bool used;
};
extern std::vector< buffer< Rectangle > > rectangles;

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

