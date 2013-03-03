#include "army.h"
#include "../Logic.h"


Army::Army( int _rectangle, int _from, int _soldiers, int _carts ) : rectangle(_rectangle), soldiers(_soldiers), carts(_carts), food_stored(0), money_stored(0), speed(1), transporting(false), stationary(true), hunger(0)
{
	from = _from;
	x = rectangles[ (int)Type::Army ].v[rectangle].x;
	y = rectangles[ (int)Type::Army ].v[rectangle].y;
	Calculate();
}

void Army::Calculate()
{
	storage_capacity = soldiers * 1 + carts * 10;
	food_consumed = (float)(soldiers + carts) * 0.04f;
	money_consumed = (float)soldiers * 0.01f;
	rectangles[ (int)Type::Army ].v[rectangle].x = x;
	rectangles[ (int)Type::Army ].v[rectangle].y = y;
	rectangles[ (int)Type::Army ].v[rectangle].scale = sqrt( (soldiers + carts) / 3.14159 );
}

Army::operator std::string()
{
	std::stringstream s;
	s << "ARMY" << std::endl << (int)(soldiers+.5f) << " SOLDIERS " << (int)(carts+.5f) << " CARTS" << std::endl << (int)(food_stored+.5f) << "# " << (int)(money_stored+.5f) << "$" << std::endl << hunger << " HUNGER" << std::endl;
	return s.str();
}

void Army::Update( Logic& l, float delta_time, int& i )
{
	// Movement
	if( !stationary )
	{
		Rectangle& r = rectangles[ (int)Type::Farm ].v[to];
		float distance = speed * delta_time;
		float d_x = r.x - x;
		float d_y = r.y - y;
		if( d_y == 0 )
			d_y = 0.0001;
		float t = Logic::L(d_x) / ( Logic::L(d_x) + Logic::L(d_y) );
		float x_ = (r.x > x ? distance : -distance) * t;
		float y_ = (r.y > y ? distance : -distance) * (1-t);
		if( Logic::L(r.x - x) <= Logic::L(x_) )
			x = r.x;
		if( Logic::L(r.y - y) <= Logic::L(y_) )
			y = r.y;

		if( x == r.x && y == r.y )
		{
			if( transporting )
			{
				bool found = false;
				for( int j(0); j < l.farms.size(); j++ )
					if( l.farms[j].rectangle == to )
					{
						if( to == transporting_to )
						{
							l.farms[j].food_contained += food_stored;
							food_stored = 0;

							final_to = from;
							from = to;
							to = l.CalculatePathTo( *this, final_to );
							found = true;
							break;
						}
						else if( to == transporting_from )
						{
							food_stored = storage_capacity;
							l.farms[j].food_contained -= food_stored;

							final_to = from;
							from = to;
							to = l.CalculatePathTo( *this, final_to );
							found = true;
							break;
						}
					}
				if( !found )
				{
					from = to;
					to = l.CalculatePathTo( *this, final_to );
				}
			}
			else
			{
				if( to == final_to )
				{
					for( int j(0); j < l.armies.size(); j++ )
						if( i != j && x == l.armies[j].x && y == l.armies[j].y )
						{
							soldiers += l.armies[j].soldiers;
							carts += l.armies[j].carts;
							rectangles[ (int)Type::Army ].erase( l.armies[j].rectangle );
							l.armies.erase( l.armies.begin() + j );
							if( j < i )
								i--;
							break;
						}
					from = to;
					stationary = true;
				}
				else
				{
					from = to;
					to = l.CalculatePathTo( *this, final_to );
				}
			}
		}
		else
		{
			x += x_;
			y += y_;
		}
		Calculate();
	}

	// If on farm, take food from farm.
	int farm = 0;
	bool on_farm = false;
	if( stationary )
		for( ; farm < l.farms.size(); farm++ )
			if( from == l.farms[farm].rectangle )
			{
				on_farm = true;
				break;
			}
	if( stationary && on_farm )
	{
		if( hunger > 0 )
		{
			l.farms[farm].food_contained -= hunger;
			hunger = 0;
		}
		l.farms[farm].food_contained -= food_consumed * delta_time;
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
					rectangles[ (int)Type::Army ].erase( l.armies[i].rectangle );
					l.armies.erase( l.armies.begin() + i );
					i--;
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

