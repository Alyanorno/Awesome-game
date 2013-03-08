#include "city.h"
#include "../Logic.h"


City::City( int _rectangle, int _point ) : rectangle(_rectangle), point(_point), money_contained(0), cart( 1, 1 ), soldier( 1, 1 ), population(0), hunger(0), used(true)
{
	Calculate();
}

void City::Calculate()
{
	size = 3.14159 * rectangles[ (int)Type::City ][rectangle].scale * rectangles[ (int)Type::City ][rectangle].scale;
	money_storage = size * 10;
	money_production = size * 2;

	population_needed = size * 100;
}

City::operator std::string()
{
	std::stringstream s;
	s << "CITY " << rectangle << std::endl << (int)(money_contained+.5f) << "/" << (int)(money_storage+.5f) << "$" << std::endl << "+" << (int)(money_production+.5f) << "$/S" << std::endl << std::endl << soldier.isProducing << " SOLDIER PRODUCTION" << std::endl << cart.isProducing << " CART PRODUCTION" << std::endl << (int)(population+.5f) << "/" << (int)(population_needed+.5f) << " PEOPLE" << std::endl;
	return s.str();
}

Army* FindArmy( Logic& l, float _x, float _y )
{
	for( int i(0); i < l.GetArmies().size(); i++ )
	{
		Army* a( &l.GetArmyByIndex( i ) );
		if( a->x == _x && a->y == _y )
			return a;
	}
	return 0;
}
void City::Update( Logic& l, float delta_time )
{
	if( !used )
		return;

	float efficency = population / population_needed;
	if( efficency > 1.f )
		efficency = 1.f;

	// Gather food from farm
	Farm& f( l.GetFarmByPoint( point ) );
	if( food_contained < food_storage * 0.95f && f.food > -0.01f )
	{
		float tax = size * 10 * delta_time * efficency;
		f.food -= tax;
		food_contained += tax;
	}

	if( food_contained <= 0 )
	{
		hunger -= l.food_per_person * population * delta_time;
	}
	else
		food_contained -= l.food_per_person * population * delta_time;

	money_contained += money_production * delta_time * efficency;
	if( money_contained > money_storage )
		money_contained = money_storage;


	if( cart.isProducing && money_contained > 0 )
	{
		money_contained -= cart.resource * delta_time * efficency;
		cart.current_time -= delta_time * efficency;
		if( cart.current_time <= 0 )
		{
			float x = rectangles[ (int)Type::City ][rectangle].x;
			float y = rectangles[ (int)Type::City ][rectangle].y;
			Army* a = FindArmy( l, x, y );
			if( a )
			{
				a->carts++;
				a->Calculate();
			}
			else
			{
				int t = rectangles[ (int)Type::Army ].insert( Rectangle( x, y, 1 ) );
				l.GetArmies().push_back( Army( t, f.rectangle, 0, 1 ) );
			}
			cart.current_time = cart.time;
		}
	}
	if( soldier.isProducing && money_contained > 0 )
	{
		money_contained -= soldier.resource * delta_time * efficency;
		soldier.current_time -= delta_time * efficency;
		if( soldier.current_time <= 0 )
		{
			float x = rectangles[ (int)Type::City ][rectangle].x;
			float y = rectangles[ (int)Type::City ][rectangle].y;
			Army* a = FindArmy( l, x, y );
			if( a )
			{
				a->soldiers++;
				a->Calculate();
			}
			else
			{
				int t = rectangles[ (int)Type::Army ].insert( Rectangle( x, y, 1 ) );
				l.GetArmies().push_back( Army( t, f.rectangle, 1, 0 ) );
			}
			soldier.current_time = soldier.time;
		}
	}


	l.PopulationCalculations( food_contained, population, hunger, delta_time );
}

