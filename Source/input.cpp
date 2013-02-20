#include "input.h"


Input::Input( Graphic& _graphic, Logic& _logic ) : graphic(_graphic), logic(_logic), mouse_wheel_z(0), mouse_wheel_scale(0), scale(1.f), line_lock(false), army_lock(false)
{}

void Input::Initialize()
{
	state = select;
	graphic.AddText( "HEJ\nDA", 0, 0, .5f );
}

void Input::Update()
{
	if( glfwGetKey( GLFW_KEY_ESC ) )
		throw exit_success();

	if( glfwGetKey( '1' ) && state != select )
	{
		if( state == build_farm || state == build_city )
			logic.RemoveTopRectangle();
		state = select;
		graphic.AddText( "HEJ\nDA", 0, 0, .5f );
	}
	else if( glfwGetKey( '2' ) && state != build_road )
	{
		if( state == select )
			graphic.RemoveTopText();
		if( state == build_farm || state == build_city )
			logic.RemoveTopRectangle();
		state = build_road;
		create = false;
	}
	else if( glfwGetKey( '3' ) && state != build_farm )
	{
		if( state == select )
			graphic.RemoveTopText();
		if( state == build_city )
			logic.RemoveTopRectangle();
		else if( state == build_road && create )
			logic.RemoveTopLine();
		state = build_farm;
		logic.AddRectangle( 0, 0, scale );
	}
	else if( glfwGetKey( '4' ) && state != build_city )
	{
		if( state == select )
			graphic.RemoveTopText();
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
	std::string temp = "";
	switch( state )
	{
		case select:
			if( army_lock )
				temp = std::to_string(army_selected) + '\n';
			closest = logic.ClosestArmy( t_x, t_y );
			if( closest.first != - 1 && closest.second <= 5 )
				temp = temp + logic.GetArmyInfo( closest.first );
			closest = logic.ClosestFarm( t_x, t_y );
			if( closest.first != - 1 && closest.second <= rectangles.v[closest.first].scale )
				temp = temp + logic.GetInfo( closest.first );
			closest = logic.ClosestCity( t_x, t_y );
			if( closest.first != - 1 && closest.second <= rectangles.v[closest.first].scale )
				temp = temp + logic.GetInfo( closest.first );
			closest = logic.ClosestStructure( t_x, t_y );
			if( closest.first != - 1 && closest.second <= rectangles.v[closest.first].scale )
				temp = temp + logic.GetInfo( closest.first );

			graphic.RemoveTopText();
			graphic.AddText( temp );
			graphic.MoveTopText( t_x, t_y );

			if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) )
			{
				if( !create )
				{
					if( !army_lock )
					{
						army_selected = logic.ClosestArmy( t_x, t_y ).first;
						if( army_selected != -1 )
							army_lock = true;
					}
					else
					{
						closest = logic.ClosestRectangle( t_x, t_y );
						if( closest.first != -1 )
							logic.ArmyTo( army_selected, closest.first );
						army_lock = false;
					}
					create = true;
				}
			}
			else if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_2 ) )
				army_lock = false;
			else
				create = false;
			break;
		case build_road:
			if( create )
			{
				closest = logic.ClosestRectangle( t_x, t_y );
				if( closest.first == -1 )
				{
					logic.RemoveTopLine();
					create = false;
					line_lock = true;
				}
				else
				{
					t_x = rectangles.v[closest.first].x;
					t_y = rectangles.v[closest.first].y;
					logic.MoveTopLine( t_x, t_y );
				}
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
					closest = logic.ClosestRectangle( t_x, t_y );
					if( closest.first != -1 )
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
			closest = logic.ClosestRectangle( t_x, t_y );
			if( closest.first != -1 )
			{
				t_x = rectangles.v[closest.first].x;
				t_y = rectangles.v[closest.first].y;
				logic.MoveTopRectangle( t_x, t_y );
			}
			else
				logic.MoveTopRectangle( 0, 0 ); // TODO: Handle this better

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

