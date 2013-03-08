#include "army.h"
#include "../Logic.h"


void Army::Move( Logic& l, float delta_time, int& i )
{
	if( to == final_to )
	{
		for( int j(0); j < l.GetArmies().size(); j++ )
		{
			Army& a( l.GetArmyByIndex(j) );
			if( a.used && i != j && x == a.x && y == a.y )
			{
				soldiers += a.soldiers;
				carts += a.carts;
				rectangles[ (int)Type::Army ].erase( a.rectangle );
				l.GetArmies().erase( j );
				break;
			}
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
			City& c( l.GetCityByIndex(i) );

			c.food_contained += food_stored;
			food_stored = 0;
		}
		else if( to == transporting_from )
		{
			if( i != -1 )
			{
				City& c( l.GetCityByIndex(i) );
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
				Farm& f( l.GetFarmByIndex( j ) );
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


Army::Army( int _rectangle, int _from, int _soldiers, int _carts ) : rectangle(_rectangle), soldiers(_soldiers), carts(_carts), people(0), food_stored(0), money_stored(0), speed(1), transporting(Resource::Nothing), state(Stationary), hunger(0), used(true)
{
	from = _from;
	x = rectangles[ (int)Type::Army ][rectangle].x;
	y = rectangles[ (int)Type::Army ][rectangle].y;
	Calculate();
}

void Army::Calculate()
{
	people_max = soldiers * 10;
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

	int farm = l.GetFarmIndex( from );
	int city = l.GetCityIndex( from );
	int structure = l.GetStructureIndex( from );

	// Feed
	if( state == Stationary && city != -1 )
	{
		City& c( l.GetCityByIndex( city ) );
		if( hunger > 0 )
		{
			c.food_contained -= hunger;
			hunger = 0;
		}
		c.food_contained -= food_consumed * delta_time;
	}
	else if( state == Stationary && farm != -1 )
	{
		Farm& f( l.GetFarmByIndex( farm ) );
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
		{
			if( hunger > 0 )
			{
				food_stored -= hunger;
				hunger = 0;
			}
			food_stored -= food_consumed * delta_time;
		}


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
	else if( state == CollectFood && farm != -1 )
	{
		Farm& f( l.GetFarmByIndex( farm ) );
		float collected = soldiers * delta_time;
		f.food -= collected;
		food_stored += collected;
		if( food_stored > storage_capacity ) // TODO: Change later when more resources can be transported
			state = Stationary;
	}
	else if( state == CollectPeople )
	{
		if( farm != -1 )
		{
			Farm& f( l.GetFarmByIndex( farm ) );
			float collected = soldiers * 10 * delta_time;
			people += collected;
			f.population -= collected;
			if( people > people_max )
			{
				people = people_max;
				state = Stationary;
			}
			else if( f.population < 0 )
			{
				f.population = 0;
				state = Stationary;
			}
		}
		else if( city != -1 )
		{
			City& c( l.GetCityByIndex( city ) );
			float collected = soldiers * 10 * delta_time;
			people += collected;
			c.population -= collected;
			if( people > people_max )
			{
				people = people_max;
				state = Stationary;
			}
			else if( c.population < 0 )
			{
				c.population = 0;
				state = Stationary;
			}
		}
		else if( structure != -1 )
		{
			Structure& s( l.GetStructureByIndex( structure ) );
			float collected = soldiers * 10 * delta_time;
			people += collected;
			s.population -= collected;
			if( people > people_max )
			{
				people = people_max;
				state = Stationary;
			}
			else if( s.population < 0 )
			{
				s.population = 0;
				state = Stationary;
			}
		}
	}
	else if( state == KillPeople )
	{
		if( structure != -1 )
		{
			Structure& s( l.GetStructureByIndex( structure ) );
			s.population -= soldiers * 100 * delta_time;
			if( s.population < 0 )
			{
				s.population = 0;
				state = Stationary;
			}
		}
		else if( city != -1 )
		{
			City& c( l.GetCityByIndex( city ) );
			c.population -= soldiers * 100 * delta_time;
			if( c.population < 0 )
			{
				c.population = 0;
				state = Stationary;
			}
		}
		else if( farm != -1 )
		{
			Farm& f( l.GetFarmByIndex( farm ) );
			f.population -= soldiers * 100 * delta_time;
			if( f.population < 0 )
			{
				f.population = 0;
				state = Stationary;
			}
		}
	}
	else if( state == DestroyFarm && farm != -1 )
	{
		Farm& f( l.GetFarmByIndex( farm ) );
		Rectangle& r( rectangles[ (int)Type::Farm ][f.rectangle] );

		float size = r.scale * r.scale;
		size -= soldiers * 0.1 * delta_time;
		if( size <= 0.1 )
		{
			l.RemoveFarm( from );
			state = Stationary;
		}
		else
		{
			r.scale = size / r.scale;
			f.Calculate();
		}
	}
	else if( state == DestroyCity && city != -1 )
	{
		City& c( l.GetCityByIndex( city ) );
		Rectangle& r( rectangles[ (int)Type::City ][c.rectangle] );

		float size = r.scale * r.scale;
		size -= soldiers * 0.1 * delta_time;
		if( size <= 0.1 )
		{
			l.RemoveCity( from );
			state = Stationary;
		}
		else
		{
			r.scale = size / r.scale;
			c.Calculate();
		}
	}
	else if( state == DestroyStructure && structure != -1 )
	{
		Structure& s( l.GetStructureByIndex( structure ) );
		Rectangle& r( rectangles[ (int)Type::Structure ][s.rectangle] );

		float size = r.scale * r.scale;
		size -= soldiers * 0.1 * delta_time;
		if( size <= 0.1 )
		{
			l.RemoveStructure( from );
			state = Stationary;
		}
		else
		{
			r.scale = size / r.scale;
			s.Calculate();
		}
	}
	else if( state == Fighting )
	{
		// TODO: Add later
	}
}

