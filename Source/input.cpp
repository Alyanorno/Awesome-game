#include "input.h"


Input::Input( Graphic& _graphic, Logic& _logic ) : graphic(_graphic), logic(_logic), mouse_wheel_z(0), mouse_wheel_scale(0), scale(1.f), line_lock(false)
{}

void Input::Initialize()
{
	state = select;
}

float Distance( float _x, float _y, float __x, float __y )
{
	return (_x - __x) * (_x - __x) + (_y - __y) * (_y - __y);
}
std::pair<int,float> ClosestRectangle( float _x, float _y )
{
	float distance = Distance( _x, _y, rectangles.v[0].x, rectangles.v[0].y );
	int closest = 0;
	for( int i(1); i < rectangles.v.size(); i++ )
	{
		float t = Distance( _x, _y, rectangles.v[i].x, rectangles.v[i].y );
		if( t < distance )
		{
			distance = t;
			closest = i; 
		}
	}
	return std::make_pair( closest, distance );
}
std::pair<float,float> ClosestLineEnd( float _x, float _y )
{
	float x, y, distance, t;
	distance = Distance( _x, _y, lines[2].start.x, lines[2].start.y );
	t = Distance( _x, _y, lines[2].end.x, lines[2].end.y );
	if( t > distance )
	{
		x = lines[2].start.x;
		y = lines[2].start.y;
	}
	else
	{
		x = lines[2].end.x;
		y = lines[2].end.y;
	}
	for( int i(3); i < lines.size(); i++ )
	{
		t = Distance( _x, _y, lines[i].start.x, lines[i].start.y );
		if( t < distance )
		{
			x = lines[i].start.x;
			y = lines[i].start.y;
		}
		t = Distance( _x, _y, lines[i].end.x, lines[i].end.y );
		if( t < distance )
		{
			x = lines[i].end.x;
			y = lines[i].end.y;
		}
	}
	return std::make_pair( x, y );
}
void Input::Update()
{
	if( glfwGetKey( GLFW_KEY_ESC ) )
		throw exit_success();

	if( glfwGetKey( '1' ) && state != select )
	{
		state = select;
	}
	else if( glfwGetKey( '2' ) && state != build_road )
	{
		if( state == build_farm || state == build_city )
			logic.RemoveTopRectangle();
		state = build_road;
		create = false;
	}
	else if( glfwGetKey( '3' ) && state != build_farm )
	{
		if( state == build_city )
			logic.RemoveTopRectangle();
		else if( state == build_road && create )
			logic.RemoveTopLine();
		state = build_farm;
		logic.AddRectangle( 0, 0, scale );
	}
	else if( glfwGetKey( '4' ) && state != build_city )
	{
		if( state == build_farm )
			logic.RemoveTopRectangle();
		else if( state == build_road && create )
			logic.RemoveTopLine();
		state = build_city;
		logic.AddRectangle( 0, 0, scale, 0 );
	}

	if( glfwGetKey( GLFW_KEY_LCTRL ) )
	{
		mouse_wheel_scale = glfwGetMouseWheel() - mouse_wheel_z;
		scale = 1 + mouse_wheel_scale / 10;
		if( state == build_farm || state == build_city )
			logic.ResizeTopRectangle( scale );
	}
	else
	{
		mouse_wheel_z = glfwGetMouseWheel() - mouse_wheel_scale;
		graphic.Zoom( mouse_wheel_z - 5.f );
	}


	int m_x, m_y;
	glfwGetMousePos( &m_x, &m_y );

	std::pair<float,float> t = graphic.GetIngameCoordinates( m_x, m_y );
	float t_x = t.first, t_y = t.second;

	std::pair<int,float> closest;
	std::pair<float,float> closest_line;
	switch( state )
	{
		case select:
			break;
		case build_road:
			if( create )
			{
				closest = ClosestRectangle( t_x, t_y );
				if( closest.second < ( rectangles.v[closest.first].scale / 2.f ) * ( rectangles.v[closest.first].scale / 2.f ) )
				{
					t_x = rectangles.v[closest.first].x;
					t_y = rectangles.v[closest.first].y;
				}
				logic.MoveTopLine( t_x, t_y );
			}

			if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_2 ) )
			{
				if( create )
				{
					logic.RemoveTopLine();
					create = false;
					line_lock = true;
				}
			}
			else if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) )
			{
				if( line_lock == true )
					break;
				if( !create )
				{
					closest = ClosestRectangle( t_x, t_y );
					logic.AddLine( rectangles.v[closest.first].x, rectangles.v[closest.first].y, 0, 0 );
				}
				create = true;
			}
			else
			{
				create = false;
				line_lock = false;
			}
			break;
		case build_farm:
			// Draw not completed farm
			closest_line = ClosestLineEnd( t_x, t_y );
			t_x = closest_line.first;
			t_y = closest_line.second;
			logic.MoveTopRectangle( t_x, t_y );

			if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) )
			{
				if( !create )
					logic.BuildFarm( t_x, t_y, scale, 1 );
				create = true;
			}
			else
				create = false;
			break;
		case build_city:
			// Draw not completed city
			closest = ClosestRectangle( t_x, t_y );
			t_x = rectangles.v[closest.first].x;
			t_y = rectangles.v[closest.first].y;
			logic.MoveTopRectangle( t_x, t_y );

			if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) )
			{
				if( !create )
					logic.BuildCity( t_x, t_y, scale, 0 );
				create = true;
			}
			else
				create = false;
			break;
	}


	float speed = 0.005f;
	if( mouse_wheel_z > 0 )
		speed += 1000 / mouse_wheel_z;
	if( glfwGetKey( 'W' ) )
		graphic.MoveY( -speed );
	if( glfwGetKey( 'S' ) )
		graphic.MoveY( speed );
	if( glfwGetKey( 'D' ) )
		graphic.MoveX( -speed );
	if( glfwGetKey( 'A' ) )
		graphic.MoveX( speed );
}

