#pragma once

#include <vector>
#include <functional>

enum class Type { Road, Farm, City, Army, Structure, Size };
enum class Resource { Nothing, Rock, Tree, Food, Gold };

template <class T>
class buffer
{
private:
	std::vector<T> v;
public:
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
	void push_back( T& _t ) { v.push_back(_t); }
	void pop_back() { v.pop_back(); }
	T& back() { return v.back(); }
	int size() { return v.size(); }
	T& operator []( int _i ) { return v[_i]; }

	void apply( std::function<void(T&)> _foo )
	{
		for( int i(0); i < v.size(); i++ )
			if( v[i].used )
				_foo( v[i] );
	}
	T& find( std::function<bool(T&)> _foo )
	{
		for( int i(0); i < v.size(); i++ )
			if( v[i].used && _foo( v[i] ) )
				return v[i];
	}

	typename std::vector<T>::iterator begin() { return v.begin(); }
	typename std::vector<T>::const_iterator begin() const { return v.begin(); }
	typename std::vector<T>::iterator end() { return v.end(); }
	typename std::vector<T>::const_iterator end() const { return v.end(); }
};

struct Rectangle
{
	Rectangle() {}
	Rectangle( float _x, float _y, float _scale, float _rotation = 0, float _scale_x = 0 )
		: x(_x), y(_y), scale(_scale), scale_x(_scale_x), rotation(_rotation), used(true) {}
	float x, y, scale, scale_x, rotation;
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

