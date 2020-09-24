#ifndef _UNIT_H_
#define _UNIT_H_

#include <Gamnet/Library/Component.h>
#include "Vector2.h"

class Unit
{
public :
	typedef Gamnet::Component Attribute;

	Unit();

	Vector2Int	position;
	int			sight;

	Attribute attributes;
};

#endif
