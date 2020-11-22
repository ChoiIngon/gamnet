#include "TileMap.h"
#include "Unit.h"

Tile::Tile()
	: type(Type::Invalid)
	, visible(false)
	, unit(nullptr)
{
}

std::shared_ptr<Tile> TileMap::GetTile(int x, int y) const
{
	if (rect.x > x || rect.y > y || rect.xMax <= x || rect.yMax <= y)
	{
		return nullptr;
	}
	return tiles[y * rect.width + x];
}

std::shared_ptr<Tile> TileMap::GetTile(const Vector2Int& coordinate) const
{
	return GetTile(coordinate.x, coordinate.y);
}

const RectInt& TileMap::SetMapSize(int width, int height)
{
	rect = RectInt(0, 0, width, height);
	tiles.clear();
	tiles.resize(width * height);
	for(int i=0; i<rect.width*rect.height; i++)
	{
		std::shared_ptr<Tile> tile = std::make_shared<Tile>();
		tile->type = Message::DungeonTileType::Invalid;
		tiles[i] = tile;
	}
	return rect;
}

const RectInt& TileMap::GetRect() const
{
	return rect;
}