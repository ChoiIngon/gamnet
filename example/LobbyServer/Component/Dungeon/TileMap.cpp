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

const RectInt& TileMap::SetRect(const RectInt& rect)
{
	this->rect = rect;
	tiles.clear();
	tiles.resize(size_t(rect.width * rect.height));
	for(int i=0; i<rect.width*rect.height; i++)
	{
		tiles[i] = std::make_shared<Tile>(Vector2Int(i % rect.width, i / rect.width));
	}
	return rect;
}

const RectInt& TileMap::GetRect() const
{
	return rect;
}