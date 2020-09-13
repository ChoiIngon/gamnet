#ifndef _TILE_H
#define _TILE_H

#include "Vector2.h"
#include <Gamnet/Library/Delegate.h>
#include <idl/MessageCommon.h>

class Tile
{
public:
	typedef Message::DungeonTileType Type;

	Tile();
public :
	Type type;
	int index;
	Vector2Int position;
	bool visible;
};

#endif
