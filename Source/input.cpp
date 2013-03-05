#include "input.h"

Input::Select::Select( Graphic& _graphic, Logic& _logic, float& _mouse_wheel )
	: State( _graphic, _logic, _mouse_wheel ), army_select_rectangle(-1), select_rectangle(-1), select( Type::Farm), lock(false), lock_cart_production(false), lock_soldier_production(false), lock_select(false)
{
	graphic.AddText( "", 0, 0, .5f );
}
Input::Select::~Select()
{
	graphic.RemoveTopText();
	if( army_select_rectangle != -1 )
		graphic.RemoveRectangle( army_select_rectangle );
	if( select_rectangle != -1 )
		graphic.RemoveRectangle( select_rectangle );
}
void Input::Select::Input( float _x, float _y )
{
	if( !lock_select )
	{
		if( glfwGetKey('Q') )
		{
			if( select == Type::Farm )
				;
			else if( select == Type::City )
				select = Type::Farm;
			else if( select == Type::Structure )
				select = Type::City;
			else if( select == Type::Army )
				select = Type::Structure;
	
			if( select_rectangle != -1 )
			{
				graphic.RemoveRectangle( select_rectangle );
				select_rectangle = -1;
			}
			lock_select = true;
		}
		if( glfwGetKey('E') )
		{
			if( select == Type::Army )
				;
			else if( select == Type::Structure )
				select = Type::Army;
			else if( select == Type::City )
				select = Type::Structure;
			else if( select == Type::Farm )
				select = Type::City;

			if( select_rectangle != -1 )
			{
				graphic.RemoveRectangle( select_rectangle );
				select_rectangle = -1;
			}
			lock_select = true;
		}
	}
	if( !glfwGetKey('Q') && !glfwGetKey('E') )
		lock_select = false;

	std::pair<int,float> closest;
	std::string temp = "";

	if( lock )
		temp = std::to_string(army_selected) + '\n';


	if( select == Type::Farm )
	{
		closest = logic.Closest( Type::Farm, _x, _y );
		if( closest.first != - 1 && closest.second <= rectangles[ (int)Type::Farm ][ logic.GetPoint( closest.first ).on_point[ Type::Farm ] ].scale )
		{
			Point& p( logic.GetPoint( closest.first ) );
			Rectangle& r( rectangles[ (int)Type::Farm ][ p.on_point[ Type::Farm ] ] );
			temp = temp + logic.GetInfo( closest.first, Type::Farm );
			if( select_rectangle == -1 )
				select_rectangle = graphic.AddRectangle( (int)Type::Structure, 0 ); // TODO: Add better texture
			graphic.MoveRectangle( select_rectangle, p.x, p.y );
			graphic.ResizeRectangle( select_rectangle, r.scale );
		}
	}
	else if( select == Type::City )
	{
		closest = logic.Closest( Type::City, _x, _y );
		if( closest.first != - 1 && closest.second <= rectangles[ (int)Type::City ][ logic.GetPoint( closest.first ).on_point[ Type::City ] ].scale )
		{
			Point& p( logic.GetPoint( closest.first ) );
			Rectangle& r( rectangles[ (int)Type::City ][ p.on_point[ Type::City ] ] );
			temp = temp + logic.GetInfo( closest.first, Type::City );
			if( !lock_soldier_production && glfwGetKey( 'R' ) )
			{
				logic.ToggleSoldierProduction( closest.first );
				lock_soldier_production = true;
			}
			if( !lock_cart_production && glfwGetKey( 'T' ) )
			{
				logic.ToggleCartProduction( closest.first );
				lock_cart_production = true;
			}

			if( select_rectangle == -1 )
				select_rectangle = graphic.AddRectangle( (int)Type::Structure, 0 ); // TODO: Add better texture
			graphic.MoveRectangle( select_rectangle, p.x, p.y );
			graphic.ResizeRectangle( select_rectangle, r.scale );
		}
		if( lock_soldier_production && !glfwGetKey( 'R' ) )
			lock_soldier_production = false;
		if( lock_cart_production && !glfwGetKey( 'T' ) )
			lock_cart_production = false;
	}
	else if( select == Type::Structure )
	{
		closest = logic.Closest( Type::Structure, _x, _y );
		if( closest.first != - 1 && closest.second <= rectangles[ (int)Type::Structure ][ logic.GetPoint( closest.first ).on_point[ Type::Structure ] ].scale )
		{
			Point& p( logic.GetPoint( closest.first ) );
			Rectangle& r( rectangles[ (int)Type::Structure ][ p.on_point[ Type::Structure ] ] );
			temp = temp + logic.GetInfo( closest.first, Type::Farm );
			if( select_rectangle == -1 )
				select_rectangle = graphic.AddRectangle( (int)Type::Structure, 0 ); // TODO: Add better texture
			graphic.MoveRectangle( select_rectangle, p.x, p.y );
			graphic.ResizeRectangle( select_rectangle, r.scale );
		}
	}
	else if( select == Type::Army )
	{
		closest = logic.ClosestArmy( _x, _y );
		if( closest.first != - 1 && closest.second <= 5 )
		{
			temp = temp + logic.GetArmyInfo( closest.first );
		}
	}

	if( temp.size() == 0 )
		if( select_rectangle != -1 )
		{
			graphic.RemoveRectangle( select_rectangle );
			select_rectangle = -1;
		}


	graphic.RemoveTopText();
	graphic.AddText( temp );
	graphic.MoveTopText( _x, _y );

	if( lock )
	{
		std::pair<float,float> t = logic.ArmyPosition( army_selected );
		if( army_select_rectangle == -1 )
			army_select_rectangle = graphic.AddRectangle( (int)Type::Structure, logic.ArmySize( army_selected ) ); // TODO: Add better texture
		graphic.MoveRectangle( army_select_rectangle, t.first, t.second );
		graphic.ResizeRectangle( army_select_rectangle, logic.ArmySize( army_selected ) );
	}

	if( select == Type::Army && glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) )
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
				closest = logic.Closest( Type::Farm, _x, _y );
				if( closest.first != -1 )
				{
					if( glfwGetKey( GLFW_KEY_LSHIFT ) )
						// Set up transport route
						logic.ArmyTransport( army_selected, closest.first, Resource::Food );
					else
						logic.ArmyTo( army_selected, closest.first );
				}
				if( army_select_rectangle != -1 )
					graphic.RemoveRectangle( army_select_rectangle );
				army_select_rectangle = -1;
				lock = false;
			}
			create = true;
		}
	}
	else if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_2 ) )
	{
		if( army_select_rectangle != -1 )
			graphic.RemoveRectangle( army_select_rectangle );
		army_select_rectangle = -1;
		lock = false;
	}
	else
		create = false;
}

Input::BuildRoad::BuildRoad( Graphic& _graphic, Logic& _logic, float& _mouse_wheel )
	: State( _graphic, _logic, _mouse_wheel ), rectangle(-1)
{}
Input::BuildRoad::~BuildRoad()
{
	if( create )
		logic.RemoveTopLine();
	if( rectangle != -1 )
		graphic.RemoveRectangle( rectangle );
}
void Input::BuildRoad::Input( float _x, float _y )
{
	std::pair<int,float> closest;

	if( create )
	{
		closest = logic.Closest( Type::Farm, _x, _y );
		if( closest.first == -1 )
		{
			logic.RemoveTopLine();
			if( rectangle != -1 )
			{
				graphic.RemoveRectangle( rectangle );
				rectangle = -1;
			}
			create = false;
		}
		else
		{
			to = closest.first;
			logic.MoveTopLine( logic.GetPoint(to).x, logic.GetPoint(to).y );
			if( rectangle == -1 )
				rectangle = graphic.AddRectangle( (int)Type::Road, 0 );
			logic.ChangeRoad( graphic.GetRectangle( rectangle ), from, to );
		}
	}

	if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_2 ) )
	{
		if( create )
		{
			logic.RemoveTopLine();
			if( rectangle != -1 )
				graphic.RemoveRectangle( rectangle );
			create = false;
		}
	}
	else if( glfwGetMouseButton( GLFW_MOUSE_BUTTON_1 ) )
	{
		if( !create )
		{
			closest = logic.Closest( Type::Farm, _x, _y );
			if( closest.first != -1 )
			{
				from = closest.first;
				logic.AddLine( logic.GetPoint(from).x, logic.GetPoint(from).y, 0, 0 );
			}
		}
		create = true;
	}
	else
	{
		if( create )
		{
			if( logic.TopLineFromEqualsTo() || logic.TopLineEqualsOtherLine() )
				logic.RemoveTopLine();
			else
				logic.BuildRoad( from, to );
			if( rectangle != -1 )
			{
				graphic.RemoveRectangle( rectangle );
				rectangle = -1;
			}
		}
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
	closest = logic.Closest( Type::Farm, _x, _y );
	if( closest.first != -1 )
	{
		_x = logic.GetPoint( closest.first ).x;
		_y = logic.GetPoint( closest.first ).y;
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

