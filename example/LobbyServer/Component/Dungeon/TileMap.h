#ifndef _TILE_MAP_H_
#define	_TILE_MAP_H_

#include <map>
#include "Tile.h"
#include "Rect.h"

class TileMap : public std::enable_shared_from_this<TileMap>
{
public :
	class Tile
	{
	public:
		typedef Message::DungeonTileType Type;

		Tile(const Vector2Int& position);
	public:
		Type type;
		const Vector2Int position;
	};

public :
	const RectInt& SetSize(int width, int height);
	const RectInt& GetRect() const;

	std::shared_ptr<Tile> GetTile(int x, int y) const;
	std::shared_ptr<Tile> GetTile(const Vector2Int& coordinate) const;
private :
	RectInt rect;
	std::vector<std::shared_ptr<Tile>> tiles;
};

#endif
