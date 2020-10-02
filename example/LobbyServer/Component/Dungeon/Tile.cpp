#include "Tile.h"
#include "Unit.h"

Tile::Tile(int index)
	: index(0)
	, type(Type::Invalid)
	, visible(false)
	, unit(nullptr)
{
}