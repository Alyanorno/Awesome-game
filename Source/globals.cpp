#include "globals.h"


HeightMap height_map( 10, 10, 1 );
std::vector< buffer< Rectangle > > rectangles( (int)Type::Size );
std::vector< Line > lines;

HeightMap::HeightMap( int _size_x, int _size_y, float _square_size )
	: x(-_size_x * _square_size / 2), y(-_size_y * _square_size / 2), size_x(_size_x), size_y(_size_y), square_size(_square_size), square_contained( size_x * size_y ), square_amount( size_x * size_y )
{
	// TODO: Generate resources on map
	int i = 0;
	while( i != 15 )
		square_contained[i++] = Resource::Wood;
	while( i != 30 )
		square_contained[i++] = Resource::Nothing;
	while( i != 45 )
		square_contained[i++] = Resource::Stone;
	while( i != 75 )
		square_contained[i++] = Resource::Wood;
	while( i != 100 )
		square_contained[i++] = Resource::Nothing;
}
void HeightMap::Update( float delta_time )
{
	// TODO: Have forest expand over time
}

