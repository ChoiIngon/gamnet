#ifndef _UNIT_H_
#define _UNIT_H_

#include <Gamnet/Library/Component.h>
#include "Unit/BehaviourTree.h"
#include "Vector2.h"

class Unit
{
public :
	typedef Gamnet::Component Attribute;

	Unit();

public :
	int max_health;
	int cur_health;
	int attack;
	int defense;
	int speed;
	int	sight;
	float critical_chance;
	float critical_rate;
	Vector2Int	position;

	std::shared_ptr<Attribute>	attributes;
};

#endif
