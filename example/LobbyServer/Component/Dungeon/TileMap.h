#ifndef _TILE_MAP_H_
#define	_TILE_MAP_H_

#include <map>
#include <memory>
#include <idl/MessageCommon.h>
#include "Rect.h"

class Unit;
class Tile
{
public:
	typedef Message::DungeonTileType Type;

	Tile();
public:
	Type type;
	bool visible;
	std::shared_ptr<Unit> unit;
};

class TileMap
{
public :
	const RectInt& SetMapSize(int width, int height);
	const RectInt& GetRect() const;

	std::shared_ptr<Tile> GetTile(int x, int y) const;
	std::shared_ptr<Tile> GetTile(const Vector2Int& coordinate) const;
private :
	RectInt rect;
	std::vector<std::shared_ptr<Tile>> tiles;
};

#endif
