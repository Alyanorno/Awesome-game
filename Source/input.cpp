#include "input.h"

Input::Select::Select( Graphic& _graphic, Logic& _logic, float& _mouse_wheel )
	: State( _graphic, _logic, _mouse_wheel ), lock(false)
	{ graphic.AddText( "HEJ\nDA", 0, 0, .5f ); }
Input::Select::~Select()
	{ graphic.RemoveTopText(); }
void Input::Select::Input( float _x, float _y )
{
	std::pair<int,float> closest;
	std::string temp = "";

	if( lock )
		temp = std::to_string(army_selected) + '\n';

	closest = logic.ClosestArmy( _x, _y );
	if( closest.first != - 1 && closest.second <= 5 )
		temp = temp + logic.GetArmyInfo( closest.first );

	closest = logic.ClosestFarm( _x, _y );
	if( closest.first != - 1 && closest.second <= rectangles.v[closest.first].scale )
		temp = temp + logic.GetInfo( closest.first );

	closest = logic.ClosestCity( _x, _y );
	if( closest.first != - 1 && closest.second <= rectangles.v[closest.first].scale )
	{
		temp = temp + logic.GetInfo( closest.first );
		if( glfwGetKey( 'E' ) )
			logic.BuildSoldiers( closest.first, 1 );
		if( glfwGetKey( 'R' ) )
			logic.BuildCarts( closest.first, 1 );
	}

	closest = logic.ClosestStructure( _x, _y );
	if( closest.first != - 1 && closest.second <= rectangles.v[closest.first].scale )
		temp = temp + logic.GetInfo( closest.first );


	graphic.RemoveTopText();
	graphic.AddText( temp );
	graphic.MoveTopText( _x, _y );


	if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) )
	{
		if( !create )
		{
			if( !lock )
			{
				army_selected = logic.ClosestArmy( _x, _y ).first;
				if( army_selected != -1 )
					lock = true;
			}
			else
			{
				closest = logic.ClosestRectangle( _x, _y );
				if( closest.first != -1 )
				{
					if( glfwGetKey( GLFW_KEY_LSHIFT ) )
						// Set up transport route
						logic.ArmyTransport( army_selected, closest.first );
					else
						logic.ArmyTo( army_selected, closest.first );
				}
				lock = false;
			}
			create = true;
		}
	}
	else if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_2 ) )
		lock = false;
	else
		create = false;
}

Input::BuildRoad::BuildRoad( Graphic& _graphic, Logic& _logic, float& _mouse_wheel )
	: State( _graphic, _logic, _mouse_wheel ), lock(false)
{}
Input::BuildRoad::~BuildRoad()
	{ if( create ) logic.RemoveTopLine(); }
void Input::BuildRoad::Input( float _x, float _y )
{
	std::pair<int,float> closest;

	if( create )
	{
		closest = logic.ClosestRectangle( _x, _y );
		if( closest.first == -1 )
		{
			logic.RemoveTopLine();
			create = false;
			lock = true;
		}
		else
		{
			_x = rectangles.v[closest.first].x;
			_y = rectangles.v[closest.first].y;
			logic.MoveTopLine( _x, _y );
		}
	}

	if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_2 ) )
	{
		if( create )
		{
			logic.RemoveTopLine();
			create = false;
			lock = true;
		}
	}
	else if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) )
	{
		if( lock == true )
			return;
		if( !create )
		{
			closest = logic.ClosestRectangle( _x, _y );
			if( closest.first != -1 )
				logic.AddLine( rectangles.v[closest.first].x, rectangles.v[closest.first].y, 0, 0 );
		}
		create = true;
	}
	else
	{
		if( create && lock )
			if( logic.TopLineFromEqualsTo() || logic.TopLineEqualsOtherLine() )
				logic.RemoveTopLine();
		create = false;
		lock = false;
	}
}

Input::BuildFarm::BuildFarm( Graphic& _graphic, Logic& _logic, float& _mouse_wheel )
	: State( _graphic, _logic, _mouse_wheel ), scale(1.f)
{
	mouse_wheel = glfwGetMouseWheel() - graphic.GetZoom();
	scale = 1 + mouse_wheel / 10;
	graphic.SetRectangle( inputRectangle, scale, (int)Textures::Farm );
}
Input::BuildFarm::~BuildFarm()
	{ graphic.SetRectangle( inputRectangle, 1, 0, false ); }
void Input::BuildFarm::Input( float _x, float _y )
{
	std::pair<int,float> closest;
	if( glfwGetKey( GLFW_KEY_LCTRL ) )
	{
		mouse_wheel = glfwGetMouseWheel() - graphic.GetZoom();
		scale = 1 + mouse_wheel / 10;
		graphic.ResizeRectangle( inputRectangle, scale );
	}

	bool overlapping = logic.OverLappingFarm( _x, _y, scale );
	graphic.SetRectangleVisibility( inputRectangle, !overlapping );

	// Draw not completed farm
	if( !overlapping )
		graphic.MoveRectangle( inputRectangle, _x, _y );

	if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) )
	{
		if( !create && !overlapping )
		{
			logic.BuildFarm( _x, _y, scale );
			create = true;
		}
	}
	else
		create = false;
}



Input::BuildCity::BuildCity( Graphic& _graphic, Logic& _logic, float& _mouse_wheel )
	: State( _graphic, _logic, _mouse_wheel ), scale(1.f)
{
	mouse_wheel = glfwGetMouseWheel() - graphic.GetZoom();
	scale = 1 + mouse_wheel / 10;
	graphic.SetRectangle( inputRectangle, scale, (int)Textures::City );
}
Input::BuildCity::~BuildCity()
	{ graphic.SetRectangle( inputRectangle, 1, 0, false ); }
void Input::BuildCity::Input( float _x, float _y )
{
	bool overlapping;
	std::pair<int,float> closest;
	if( glfwGetKey( GLFW_KEY_LCTRL ) )
	{
		mouse_wheel = glfwGetMouseWheel() - graphic.GetZoom();
		scale = 1 + mouse_wheel / 10;
		graphic.ResizeRectangle( inputRectangle, scale );
	}

	// Draw not completed city
	closest = logic.ClosestRectangle( _x, _y );
	if( closest.first != -1 )
	{
		_x = rectangles.v[closest.first].x;
		_y = rectangles.v[closest.first].y;
		overlapping = logic.OverLappingCity( _x, _y, scale );
		if( !overlapping )
			graphic.MoveRectangle( inputRectangle, _x, _y );
	}
	else
		overlapping = true;

	graphic.SetRectangleVisibility( inputRectangle, !overlapping );

	if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) )
	{
		if( !create && !overlapping )
		{
			logic.BuildCity( _x, _y, scale );
			create = true;
		}
	}
	else
		create = false;
}


Input::Input( Graphic& _graphic, Logic& _logic ) : graphic(_graphic), logic(_logic), mouse_wheel(0)
{
	state = std::unique_ptr<State>( new Select( graphic, logic, mouse_wheel ) );
}

void Input::Initialize()
{}

void Input::Update()
{
	if( glfwGetKey( GLFW_KEY_ESC ) )
		throw exit_success();

	if( glfwGetKey( '1' ) && !dynamic_cast<Select*>( state.get() ) )
	{
		state.reset();
		state = std::unique_ptr<State>( new Select( graphic, logic, mouse_wheel ) );
	}
	else if( glfwGetKey( '2' ) && !dynamic_cast<BuildRoad*>( state.get() ) )
	{
		state.reset();
		state = std::unique_ptr<State>( new BuildRoad( graphic, logic, mouse_wheel ) );
	}
	else if( glfwGetKey( '3' ) && !dynamic_cast<BuildFarm*>( state.get() ) )
	{
		state.reset();
		state = std::unique_ptr<State>( new BuildFarm( graphic, logic, mouse_wheel ) );
	}
	else if( glfwGetKey( '4' ) && !dynamic_cast<BuildCity*>( state.get() ) )
	{
		state.reset();
		state = std::unique_ptr<State>( new BuildCity( graphic, logic, mouse_wheel ) );
	}

	if( !glfwGetKey( GLFW_KEY_LCTRL ) )
		graphic.Zoom( glfwGetMouseWheel() - mouse_wheel );


	int m_x, m_y;
	glfwGetMousePos( &m_x, &m_y );
	std::pair<float,float> t = graphic.GetIngameCoordinates( m_x, m_y );


	state->Input( t.first, t.second );


	float speed = 0.005f;
	if( graphic.GetZoom() > 0 )
		speed += speed * graphic.GetZoom();
	if( glfwGetKey( 'W' ) )
		graphic.MoveY( -speed );
	if( glfwGetKey( 'S' ) )
		graphic.MoveY( speed );
	if( glfwGetKey( 'D' ) )
		graphic.MoveX( -speed );
	if( glfwGetKey( 'A' ) )
		graphic.MoveX( speed );
}

