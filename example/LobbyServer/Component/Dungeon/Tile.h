#ifndef _TILE_H
#define _TILE_H

#include "Vector2.h"
#include <Gamnet/Library/Delegate.h>
#include <idl/MessageCommon.h>
#include <memory>

class Unit;
class Tile
{
public:
	typedef Message::DungeonTileType Type;

	Tile(int index);
public :
	const int index;
	Type type;
	Vector2Int position;
	bool visible;
	std::shared_ptr<Unit> unit;
};

#endif
