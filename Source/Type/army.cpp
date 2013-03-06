#include "army.h"
#include "../Logic.h"


void Army::Move( Logic& l, float delta_time, int& i )
{
	if( to == final_to )
	{
		for( int j(0); j < l.GetArmies().size(); j++ )
			if( l.GetArmy(j).used && i != j && x == l.GetArmy(j).x && y == l.GetArmy(j).y )
			{
				soldiers += l.GetArmy(j).soldiers;
				carts += l.GetArmy(j).carts;
				rectangles[ (int)Type::Army ].erase( l.GetArmy(j).rectangle );
				l.GetArmies().erase( j );
				break;
			}
		from = to;
		state = Stationary;
	}
	else
	{
		from = to;
		to = l.CalculatePathTo( *this, final_to );
	}
}
void Army::Transport( Logic& l, float delta_time )
{
	if( to == final_to )
	{
		int i = l.GetCityIndex( to );
		if( to == transporting_to )
		{
			if( i == -1 )
				throw std::string("Must transport to a city");
			City& c( l.GetCity(i) );

			c.food_contained += food_stored;
			food_stored = 0;
		}
		else if( to == transporting_from )
		{
			if( i != -1 )
			{
				City& c( l.GetCity(i) );
				if( hunger > 0 )
				{
					c.food_contained -= hunger;
					hunger = 0;
				}

				food_stored = storage_capacity;
				c.food_contained -= food_stored;
			}
			else
			{
				int j = l.GetFarmIndex( to );
				if( j == -1 )
					throw std::string("Must transport from a city or farm");
				Farm& f( l.GetFarm( j ) );
				if( hunger > 0 )
				{
					f.food -= hunger;
					hunger = 0;
				}

				food_stored = storage_capacity;
				f.food -= food_stored;
			}
		}
		final_to = from;
		from = to;
		to = l.CalculatePathTo( *this, final_to );
	}
	else
	{
		from = to;
		to = l.CalculatePathTo( *this, final_to );
	}
}


Army::Army( int _rectangle, int _from, int _soldiers, int _carts ) : rectangle(_rectangle), soldiers(_soldiers), carts(_carts), food_stored(0), money_stored(0), speed(1), transporting(Resource::Nothing), state(Stationary), hunger(0), used(true)
{
	from = _from;
	x = rectangles[ (int)Type::Army ][rectangle].x;
	y = rectangles[ (int)Type::Army ][rectangle].y;
	Calculate();
}

void Army::Calculate()
{
	storage_capacity = soldiers * 1 + carts * 10;
	food_consumed = (float)(soldiers + carts) * 0.04f;
	money_consumed = (float)soldiers * 0.01f;
	rectangles[ (int)Type::Army ][rectangle].x = x;
	rectangles[ (int)Type::Army ][rectangle].y = y;
	rectangles[ (int)Type::Army ][rectangle].scale = sqrt( (soldiers + carts) / 3.14159 );
}

Army::operator std::string()
{
	std::stringstream s;
	s << "ARMY" << std::endl << (int)(soldiers+.5f) << " SOLDIERS " << (int)(carts+.5f) << " CARTS" << std::endl << (int)(food_stored+.5f) << "# " << (int)(money_stored+.5f) << "$" << std::endl << hunger << " HUNGER" << std::endl;
	return s.str();
}

void Army::Update( Logic& l, float delta_time, int i )
{
	if( !used )
		return;

	// Movement
	if( state == Moving )
	{
		Point& p = l.GetPoint( to );
		float distance = speed * delta_time;
		float d_x = p.x - x;
		float d_y = p.y - y;
		if( d_y == 0 )
			d_y = 0.0001;
		float t = Logic::L(d_x) / ( Logic::L(d_x) + Logic::L(d_y) );
		float x_ = (p.x > x ? distance : -distance) * t;
		float y_ = (p.y > y ? distance : -distance) * (1-t);
		if( Logic::L(p.x - x) <= Logic::L(x_) )
			x = p.x;
		if( Logic::L(p.y - y) <= Logic::L(y_) )
			y = p.y;

		if( x == p.x && y == p.y )
		{
			if( transporting != Resource::Nothing )
				Transport( l, delta_time );
			else
				Move( l, delta_time, i );
		}
		else
		{
			x += x_;
			y += y_;
		}
		Calculate();
	}

	int city = l.GetCityIndex( from );
	int farm = l.GetFarmIndex( from );
	if( state == Stationary && city != -1 )
	{
		City& c( l.GetCity( city ) );
		if( hunger > 0 )
		{
			c.food_contained -= hunger;
			hunger = 0;
		}
		c.food_contained -= food_consumed * delta_time;
	}
	else if( state == Stationary && farm != -1 )
	{
		Farm& f( l.GetFarm( farm ) );
		if( hunger > 0 )
		{
			f.food -= hunger;
			hunger = 0;
		}
		f.food -= food_consumed * delta_time;
	}
	else
		if( food_stored <= 0 )
		{
			hunger += food_consumed * delta_time;
			float limit = soldiers + carts;
			if( hunger > limit )
			{
				// Start the starving... muahahahaha.
				if( soldiers )
					soldiers -= soldiers * 0.1 <= 1 ? 1: soldiers * 0.1;
				if( carts )
					carts -= carts * 0.2 <= 1 ? 1: carts * 0.1;
				limit = soldiers + carts;
				hunger = limit - limit / 10;
				Calculate();

				if( soldiers + carts < 1 )
				{
					rectangles[ (int)Type::Army ].erase( l.GetArmies()[i].rectangle );
					l.GetArmies().erase( i );
					return;
				}
			}
		}
		else
			food_stored -= food_consumed * delta_time;


	if( money_stored <= 0 )
	{
		// TODO: They dont like you... MUTINY.
	}
	else
		money_stored -= money_consumed * delta_time;
}

