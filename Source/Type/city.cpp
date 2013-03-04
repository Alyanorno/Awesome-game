#include "city.h"
#include "../Logic.h"


City::City( int _rectangle, int _farm_rectangle ) : rectangle(_rectangle), farm_rectangle(_farm_rectangle), money_contained(0), producing_carts(0), cart_production_time(1), current_cart_production( cart_production_time ), cart_money(1), producing_soldiers(0), soldier_production_time(1), current_soldier_production( soldier_production_time ), soldier_money(1), hunger(0)
{
	Calculate();
}

void City::Calculate()
{
	float size = 3.14159 * rectangles[ (int)Type::City ].v[rectangle].scale * rectangles[ (int)Type::City ].v[rectangle].scale;
	money_storage = size * 10;
	money_production = size * 2;

	population_needed = size * 100;
	population = population_needed; // temp
}

City::operator std::string()
{
	std::stringstream s;
	s << "CITY " << rectangle << std::endl << (int)(money_contained+.5f) << "/" << (int)(money_storage+.5f) << "$" << std::endl << "+" << (int)(money_production+.5f) << "$/S" << std::endl << std::endl << producing_soldiers << " SOLDIER PRODUCTION" << std::endl << producing_carts << " CART PRODUCTION" << std::endl << (int)(population+.5f) << "/" << (int)(population_needed+.5f) << " PEOPLE" << std::endl;
	return s.str();
}

void City::Update( Logic& l, float delta_time )
{
	float efficency = population / population_needed;
	if( efficency > 1.f )
		efficency = 1.f;

	Farm& f( l.GetFarm( farm_rectangle ) );
	if( f.food_contained <= 0 )
	{
		hunger -= l.food_per_person * population * delta_time;
	}
	else
		f.food_contained -= l.food_per_person * population * delta_time;

	money_contained += money_production * delta_time * efficency;
	if( money_contained > money_storage )
		money_contained = money_storage;
#define PRODUCE( NAME, SOLDIER, CART ) \
	if( producing_##NAME##s ) \
	{ \
		money_contained -= NAME##_money * delta_time * efficency; \
		current_##NAME##_production -= delta_time * efficency; \
		if( current_##NAME##_production <= 0 ) \
		{ \
			float x = rectangles[ (int)Type::City ].v[rectangle].x; \
			float y = rectangles[ (int)Type::City ].v[rectangle].y; \
			bool army_in_city = false; \
			for( int k(0); k < l.GetArmies().size(); k++ ) \
			{ \
				Army& a( l.GetArmy( k ) ); \
				if( a.x == x && a.y == y ) \
				{ \
					army_in_city = true; \
					a.##NAME##s++; \
					a.Calculate(); \
					break; \
				} \
			} \
			\
			if( !army_in_city ) \
			{ \
				int t = rectangles[ (int)Type::Army ].insert( Rectangle( x, y, 1 ) ); \
				l.GetArmies().push_back( Army( t, f.rectangle, SOLDIER, CART ) ); \
			} \
			current_##NAME##_production = NAME##_production_time; \
		} \
	}
	if( money_contained > 0 )
	{
		PRODUCE( cart, 0, 1 );
		PRODUCE( soldier, 1, 0 );
	}
#undef PRODUCE

	float& food_contained( f.food_contained );
	l.PopulationCalculations( food_contained, population, hunger, delta_time );
}

