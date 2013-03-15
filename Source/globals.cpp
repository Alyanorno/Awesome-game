#include "globals.h"


HeightMap height_map( 10, 10, 1 );
std::vector< buffer< Rectangle > > rectangles( (int)Type::Size );
std::vector< Line > lines;

HeightMap::HeightMap( int _size_x, int _size_y, float _square_size )
	: x(-_size_x * _square_size / 2), y(-_size_y * _square_size / 2), size_x(_size_x), size_y(_size_y), square_size(_square_size), square_contained( size_x * size_y ), square_amount( size_x * size_y )
{
	// TODO: Generate resources on map

	// Temp code
	for( int i(0); i < 15; i++ )
	{
		square_contained[i] = Resource::Wood;
		square_amount[i] = 100;
	}
	for( int i(15); i < 30; i++ )
	{
		square_contained[i] = Resource::Nothing;
		square_amount[i] = 0;
	}
	for( int i(30); i < 45; i++ )
	{
		square_contained[i] = Resource::Stone;
		square_amount[i] = 100;
	}
	for( int i(45); i < 75; i++ )
	{
		square_contained[i] = Resource::Wood;
		square_amount[i] = 100;
	}
	for( int i(75); i < 100; i++ )
	{
		square_contained[i] = Resource::Nothing;
		square_amount[i] = 0;
	}
}
void HeightMap::Update( float delta_time )
{
	// TODO: Have forest expand over time
}

