#include "logic.h"

void Logic::AddLine( float _x, float _y, float __x, float __y )
{
	lines.push_back( Line( _x, _y, __x, __y ) );
}

void Logic::AddRectangle( float _x, float _y, float _scale, int _texture )
{ 
	rectangles.push_back( Rectangle( _x, _y, _scale, _texture ) );
}

void Logic::RemoveLine( int _i )
{
	lines.erase( lines.begin() + _i );
}

void Logic::RemoveRectangle( int _i )
{
	rectangles.erase( rectangles.begin() + _i );
}

void Logic::RemoveTopLine()
{
	if( lines.size() <= 2 ) return; lines.pop_back();
}
void Logic::RemoveTopRectangle()
{
	rectangles.pop_back();
}

void Logic::ResizeRectangle( int _i, float _scale )
{
	rectangles[_i].scale = _scale;
}

void Logic::MoveTopLine( float _x, float _y )
{
	if( lines.size() > 2 ) lines.back().end = Line::Coords( _x, _y );
}

void Logic::MoveTopRectangle( float _x, float _y )
{
	rectangles.back().x = _x; rectangles.back().y = _y;
}

void Logic::ResizeTopRectangle( float _scale )
{
	rectangles.back().scale = _scale;
}

void Logic::Initialize()
{
	rectangles.push_back( Rectangle( 0, 0, 1, 1 ) );
}

void Logic::Update()
{
}

