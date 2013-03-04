#include "input.h"

Input::Select::Select( Graphic& _graphic, Logic& _logic, float& _mouse_wheel )
	: State( _graphic, _logic, _mouse_wheel ), select_rectangle(-1), lock(false), lock_cart_production(false), lock_soldier_production(false)
{
	graphic.AddText( "", 0, 0, .5f );
}
Input::Select::~Select()
{
	graphic.RemoveTopText();
	if( select_rectangle != -1 )
		graphic.RemoveRectangle( select_rectangle );
}
void Input::Select::Input( float _x, float _y )
{
	std::pair<int,float> closest;
	std::string temp = "";

	if( lock )
		temp = std::to_string(army_selected) + '\n';

	closest = logic.ClosestArmy( _x, _y );
	if( closest.first != - 1 && closest.second <= 5 )
		temp = temp + logic.GetInfo( closest.first, Type::Army );

	closest = logic.ClosestFarm( _x, _y );
	if( closest.first != - 1 && closest.second <= rectangles[ (int)Type::Farm ].v[closest.first].scale )
		temp = temp + logic.GetInfo( closest.first, Type::Farm );

	closest = logic.ClosestCity( _x, _y );
	if( closest.first != - 1 && closest.second <= rectangles[ (int)Type::City ].v[closest.first].scale )
	{
		temp = temp + logic.GetInfo( closest.first, Type::City );
		if( !lock_soldier_production && glfwGetKey( 'E' ) )
		{
			logic.ToggleSoldierProduction( closest.first );
			lock_soldier_production = true;
		}
		if( !lock_cart_production && glfwGetKey( 'R' ) )
		{
			logic.ToggleCartProduction( closest.first );
			lock_cart_production = true;
		}
	}
	if( lock_soldier_production && !glfwGetKey( 'E' ) )
		lock_soldier_production = false;
	if( lock_cart_production && !glfwGetKey( 'E' ) )
		lock_cart_production = false;

	closest = logic.ClosestStructure( _x, _y );
	if( closest.first != - 1 && closest.second <= rectangles[ (int)Type::Structure ].v[closest.first].scale )
		temp = temp + logic.GetInfo( closest.first, Type::Structure );


	graphic.RemoveTopText();
	graphic.AddText( temp );
	graphic.MoveTopText( _x, _y );

	if( lock )
	{
		std::pair<float,float> t = logic.ArmyPosition( army_selected );
		if( select_rectangle == -1 )
			select_rectangle = graphic.AddRectangle( (int)Type::Structure, logic.ArmySize( army_selected ) ); // TODO: Add better texture
		graphic.MoveRectangle( select_rectangle, t.first, t.second );
		graphic.ResizeRectangle( select_rectangle, logic.ArmySize( army_selected ) );
	}

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
				closest = logic.ClosestFarm( _x, _y );
				if( closest.first != -1 )
				{
					if( glfwGetKey( GLFW_KEY_LSHIFT ) )
						// Set up transport route
						logic.ArmyTransport( army_selected, closest.first, Resource::Food );
					else
						logic.ArmyTo( army_selected, closest.first );
				}
				if( select_rectangle != -1 )
					graphic.RemoveRectangle( select_rectangle );
				select_rectangle = -1;
				lock = false;
			}
			create = true;
		}
	}
	else if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_2 ) )
	{
		if( select_rectangle != -1 )
			graphic.RemoveRectangle( select_rectangle );
		select_rectangle = -1;
		lock = false;
	}
	else
		create = false;
}

Input::BuildRoad::BuildRoad( Graphic& _graphic, Logic& _logic, float& _mouse_wheel )
	: State( _graphic, _logic, _mouse_wheel )
{}
Input::BuildRoad::~BuildRoad()
	{ if( create ) logic.RemoveTopLine(); }
void Input::BuildRoad::Input( float _x, float _y )
{
	std::pair<int,float> closest;

	if( create )
	{
		closest = logic.ClosestFarm( _x, _y );
		if( closest.first == -1 )
		{
			logic.RemoveTopLine();
			create = false;
		}
		else
		{
			to = closest.first;
			_x = rectangles[ (int)Type::Farm ].v[to].x;
			_y = rectangles[ (int)Type::Farm ].v[to].y;
			logic.MoveTopLine( _x, _y );
		}
	}

	if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_2 ) )
	{
		if( create )
		{
			logic.RemoveTopLine();
			create = false;
		}
	}
	else if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) )
	{
		if( !create )
		{
			closest = logic.ClosestFarm( _x, _y );
			if( closest.first != -1 )
			{
				from = closest.first;
				logic.AddLine( rectangles[ (int)Type::Farm ].v[from].x, rectangles[ (int)Type::Farm ].v[from].y, 0, 0 );
			}
		}
		create = true;
	}
	else
	{
		if( create )
			if( logic.TopLineFromEqualsTo() || logic.TopLineEqualsOtherLine() )
				logic.RemoveTopLine();
			else
				logic.BuildRoad( from, to );
		create = false;
	}
}

Input::BuildFarm::BuildFarm( Graphic& _graphic, Logic& _logic, float& _mouse_wheel )
	: State( _graphic, _logic, _mouse_wheel ), scale(1.f)
{
	mouse_wheel = glfwGetMouseWheel() - graphic.GetZoom();
	scale = 1 + mouse_wheel / 10;
	rectangle = graphic.AddRectangle( (int)Type::Farm, scale );
}
Input::BuildFarm::~BuildFarm()
	{ if( rectangle != -1 ) graphic.RemoveRectangle( rectangle ); }
void Input::BuildFarm::Input( float _x, float _y )
{
	std::pair<int,float> closest;
	if( glfwGetKey( GLFW_KEY_LCTRL ) )
	{
		mouse_wheel = glfwGetMouseWheel() - graphic.GetZoom();
		scale = 1 + mouse_wheel / 10;
		if( rectangle != -1 )
			graphic.ResizeRectangle( rectangle, scale );
	}

	if( logic.OverLappingFarm( _x, _y, scale ) )
	{
		if( rectangle != -1 )
		{
			graphic.RemoveRectangle( rectangle );
			rectangle = -1;
		}
	}
	else
		if( rectangle == -1 )
			rectangle = graphic.AddRectangle( (int)Type::Farm, scale );

	// Draw not completed farm
	if( rectangle != -1 )
		graphic.MoveRectangle( rectangle, _x, _y );

	if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) )
	{
		if( !create && rectangle != -1 )
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
	rectangle = graphic.AddRectangle( (int)Type::City, scale );
}
Input::BuildCity::~BuildCity()
	{ if( rectangle != -1 ) graphic.RemoveRectangle( rectangle ); }
void Input::BuildCity::Input( float _x, float _y )
{
	bool overlapping;
	std::pair<int,float> closest;
	if( glfwGetKey( GLFW_KEY_LCTRL ) )
	{
		mouse_wheel = glfwGetMouseWheel() - graphic.GetZoom();
		scale = 1 + mouse_wheel / 10;
		if( rectangle != -1 )
			graphic.ResizeRectangle( rectangle, scale );
	}

	// Draw not completed city
	closest = logic.ClosestFarm( _x, _y );
	if( closest.first != -1 )
	{
		_x = rectangles[ (int)Type::Farm ].v[closest.first].x;
		_y = rectangles[ (int)Type::Farm ].v[closest.first].y;
		if( !logic.OverLappingCity( _x, _y, scale ) )
		{
			if( rectangle == -1 )
				rectangle = graphic.AddRectangle( (int)Type::City, scale );
			graphic.MoveRectangle( rectangle, _x, _y );
		}
		else
			if( rectangle != -1 )
			{
				graphic.RemoveRectangle( rectangle );
				rectangle = -1;
			}
	}
	else
		if( rectangle != -1 )
		{
			graphic.RemoveRectangle( rectangle );
			rectangle = -1;
		}


	if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) )
	{
		if( !create && rectangle != -1 )
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

