#include "Unit.h"

Unit::Unit()
	: max_health(0)
	, cur_health(0)
	, attack(0)
	, defense(0)
	, speed(0)
	, sight(0)
	, critical_chance(0.0f)
	, critical_rate(0.0f)
	, attributes(std::make_shared<Attribute>())
{
}