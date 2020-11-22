#include "TileMap.h"

TileMap::Tile::Tile(const Vector2Int& position)
	: position(position)
{
}

std::shared_ptr<TileMap::Tile> TileMap::GetTile(int x, int y) const
{
	if (rect.x > x || rect.y > y || rect.xMax <= x || rect.yMax <= y)
	{
		return nullptr;
	}
	return tiles[size_t(y * rect.width + x)];
}

std::shared_ptr<TileMap::Tile> TileMap::GetTile(const Vector2Int& coordinate) const
{
	return GetTile(coordinate.x, coordinate.y);
}

const RectInt& TileMap::SetSize(int width, int height)
{
	rect = RectInt(0, 0, width, height);
	tiles.clear();
	tiles.resize((size_t)(width * height));
	for(int i=0; i<rect.width*rect.height; i++)
	{
		std::shared_ptr<Tile> tile = std::make_shared<Tile>(Vector2Int(i % rect.width, i / rect.width));
		tile->type = Message::DungeonTileType::Invalid;
		tiles[i] = tile;
	}
	return rect;
}

const RectInt& TileMap::GetRect() const
{
	return rect;
}