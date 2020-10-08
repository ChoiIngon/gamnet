#include "Dungeon.h"
#include "Unit.h"
#include <Gamnet/Library/Random.h>
#include <iostream>

namespace Component {
void Dungeon::Block::AddNeighbor(std::shared_ptr<Block> block)
{
	auto itr = std::find_if(neighbors.begin(), neighbors.end(), [block](const std::shared_ptr<Block>& neighbor) { return neighbor->id == block->id; });
	if(itr == neighbors.end())
	{
		neighbors.push_back(block);
	}
}

Dungeon::Dungeon()
	: room_count(0)
	, min_room_width(0)
	, max_room_width(0)
	, min_room_height(0)
	, max_room_height(0)
	, min_distance(0)
	, start(nullptr)
	, end(nullptr)
{
}

void Dungeon::Init()
{
	room_count = std::max(room_count, 1);
	min_room_width = std::max(min_room_width, 3);
	max_room_width = std::max(max_room_width, 3);
	min_room_height = std::max(min_room_height, 3);
	max_room_height = std::max(max_room_height, 3);
	min_distance = std::max(min_distance, 1);

	InitBlocks();
	InitTiles();
	BuildPath();
	BuildWall();

	start = blocks[0];
	end = blocks[0];

	for(auto block : blocks)
	{
		if (Block::Type::Room == block->type)
		{
			if (start->distance < block->distance)
			{
				start = block;
			}
		}
	}

	for(auto block : blocks)
	{
		if (Block::Type::Room == block->type)
		{
			if (start->id == block->id)
			{
				continue;
			}

			std::list<std::shared_ptr<Block>> path = FindPath(start, block);
			if (end->distance < path.size())
			{
				end->distance = path.size();
				end = block;
			}
		}
	}

	tiles[(int)start->rect.Center().y * rect.width + (int)start->rect.Center().x]->type = Tile::Type::StairUp;
	tiles[(int)end->rect.Center().y * rect.width + (int)end->rect.Center().x]->type = Tile::Type::StairDown;
}

std::shared_ptr<Tile> Dungeon::GetTile(int x, int y) const
{
	if (rect.x > x || rect.y > y || rect.xMax <= x || rect.yMax <= y)
	{
		return nullptr;
	}
	return tiles[y * rect.width + x];
}

std::shared_ptr<Tile> Dungeon::GetTile(const Vector2Int& coordinate) const
{
	return GetTile(coordinate.x, coordinate.y);
}

void Dungeon::InitBlocks()
{
	int blockID = 1;

	for(int i=0; i<room_count; i++)
	{
		std::shared_ptr<Block> block = std::make_shared<Block>(blockID++);
		block->type = Block::Type::Room;
		block->rect.width = Gamnet::Random::Range(min_room_width, max_room_width - 1);
		block->rect.height = Gamnet::Random::Range(min_room_height, max_room_height - 1);

		MoveToEmptySpace(block);

		int xMax = rect.xMax;
		int yMax = rect.yMax;
		rect.x = std::min(rect.x, block->rect.x);
		rect.y = std::min(rect.y, block->rect.y);
		rect.xMax = std::max(xMax, (int)block->rect.xMax);
		rect.yMax = std::max(yMax, (int)block->rect.yMax);
		blocks.push_back(block);
	}

	for(auto block : blocks)
	{
		block->rect.x -= rect.x;
		block->rect.y -= rect.y;
	}

	rect.x = 0;
	rect.y = 0;

	for(int y=0; y<rect.yMax; y++)
	{
		for(int x=0; x<rect.xMax; x++)
		{
			std::shared_ptr<Block> block = std::make_shared<Block>(blockID++);
			block->type = Block::Type::Corridor;
			block->rect.x = x;
			block->rect.y = y;
			block->rect.width = 1;
			block->rect.height = 1;

			int width = Gamnet::Random::Range(min_room_width, max_room_width - 1);
			int height = Gamnet::Random::Range(min_room_height, max_room_height - 1);

			if(false == OverlapWithExistBlocks(block))
			{
				if(0 == Gamnet::Random::Range(0, 1))
				{
					while(height > block->rect.height && (int)rect.yMax > (int)block->rect.yMax)
					{
						block->rect.yMax = block->rect.yMax + 1;
						if(true == OverlapWithExistBlocks(block))
						{
							block->rect.yMax = block->rect.yMax - 1;
							break;
						}
					}

					while (width > block->rect.width && (int)rect.xMax > (int)block->rect.xMax)
					{
						block->rect.xMax = block->rect.xMax + 1;
						if (true == OverlapWithExistBlocks(block))
						{
							block->rect.xMax = block->rect.xMax - 1;
							break;
						}
					}
				}
				else
				{
					while (width > block->rect.width && (int)rect.xMax > (int)block->rect.xMax)
					{
						block->rect.xMax = block->rect.xMax + 1;
						if (true == OverlapWithExistBlocks(block))
						{
							block->rect.xMax = block->rect.xMax - 1;
							break;
						}
					}

					while (height > block->rect.height && (int)rect.yMax > (int)block->rect.yMax)
					{
						block->rect.yMax = block->rect.yMax + 1;
						if (true == OverlapWithExistBlocks(block))
						{
							block->rect.yMax = block->rect.yMax - 1;
							break;
						}
					}
				}

				/*
				if (0 == block->id % 3 && min_room_width <= block->rect.width && min_room_height <= block->rect.height)
				{
					block->type = Block::Type::Room;
				}
				*/
				blocks.push_back(block);
			}
		}
	}

	for(auto block : blocks)
	{
		block->rect.x += 1;
		block->rect.y += 1;
	}
	rect.width += 2;
	rect.height += 2;
}

void Dungeon::InitTiles()
{
	tiles.resize((size_t)rect.width * rect.height);

	for (int i = 0; i < (int)tiles.size(); i++)
	{
		std::shared_ptr<Tile> tile = std::make_shared<Tile>(i);
		tile->position = Vector2Int(tile->index % rect.width, tile->index / rect.width);
		tiles[i] = tile;
	}

	for(auto block : blocks)
	{
		for (int x = block->rect.x; x < block->rect.xMax; x++)
		{
			tiles[block->rect.y * rect.width + x]->type = Tile::Type::Wall;
			tiles[(block->rect.yMax - 1) * rect.width + x]->type = Tile::Type::Wall;
		}

		for (int y = block->rect.y; y < block->rect.yMax; y++)
		{
			tiles[y * rect.width + block->rect.x]->type = Tile::Type::Wall;
			tiles[y * rect.width + block->rect.xMax - 1]->type = Tile::Type::Wall;
		}

		if (Block::Type::Room == block->type)
		{
			tiles[block->rect.y * rect.width + block->rect.x]->type = Tile::Type::Invalid;
			tiles[block->rect.y * rect.width + block->rect.xMax - 1]->type = Tile::Type::Invalid;
			tiles[(block->rect.yMax - 1) * rect.width + block->rect.x]->type = Tile::Type::Invalid;
			tiles[(block->rect.yMax - 1) * rect.width + block->rect.xMax - 1]->type = Tile::Type::Invalid;
		}
	}
}

void Dungeon::BuildPath()
{
	std::vector<std::shared_ptr<Edge>> edges;
	for(auto block : blocks)
	{
		std::shared_ptr<Edge> edge = std::make_shared<Edge>();
		edge->id = block->id;
		edge->parent = 0;
		edge->distance = std::numeric_limits<int>::max();
		edge->visit = false;
		edges.push_back(edge);
	}

	std::shared_ptr<Edge> edge = edges[0];
	edge->distance = 0;

	while (true)
	{
		std::sort(edges.begin(), edges.end(), [] (const std::shared_ptr<Edge>& lhs, const std::shared_ptr<Edge>& rhs)
		{
			return lhs->distance < rhs->distance;
		});

		bool finish = true;
		int visitCount = 0;
		for (int i = 0; i < (int)edges.size() - visitCount;)
		{
			std::shared_ptr<Edge> e = edges[i];
			if (true == e->visit)
			{
				edges.erase(edges.begin() + i);
				edges.push_back(e);
				visitCount++;
			}
			else
			{
				i++;
				finish = false;
			}
		}

		if (true == finish)
		{
			break;
		}

		edge = edges[0];
		edge->visit = true;
		std::list<std::shared_ptr<Dungeon::Block>> neighbors = FindNeighborBlocks(edge->id);
		for(auto neighbor : neighbors)
		{
			auto itr = std::find_if(edges.begin(), edges.end(), [neighbor](const std::shared_ptr<Edge>& e) { return e->id == neighbor->id; });
			std::shared_ptr<Edge> next = *itr;
			int distance = std::min(next->distance, edge->distance + Gamnet::Random::Range(1, 1000));
			if (distance < next->distance)
			{
				next->parent = edge->id;
				next->distance = distance;
			}
		}
	}

	for(auto e : edges)
	{
		std::shared_ptr<Block> parent = FindBlock(e->parent);
		if (nullptr == parent)
		{
			continue;
		}
		std::shared_ptr<Block> child = FindBlock(e->id);
		parent->AddNeighbor(child);
		child->AddNeighbor(parent);
	}

	std::shared_ptr<Block> start = FindBlock(1);
	for(auto block : blocks)
	{
		if (start->id == block->id)
		{
			continue;
		}

		if (Block::Type::Corridor == block->type)
		{
			continue;
		}

		std::list<std::shared_ptr<Block>> path = FindPath(start, block);
		std::shared_ptr<Block> from = start;
		int distance = 1;
		while (0 < path.size())
		{
			std::shared_ptr<Block> to = path.front();
			to->distance = distance++;
			FindDoor(from, to);
			from = path.front();
			path.pop_front();
		}
	}
}

std::list<std::shared_ptr<Dungeon::Block>> Dungeon::FindPath(std::shared_ptr<Block> from, std::shared_ptr<Block> to)
{
	std::set<int> vistedTileIndices;
	auto path = FindPath(from, to, vistedTileIndices);
	if(nullptr == path)
	{
		return std::list<std::shared_ptr<Dungeon::Block>>();
	}
	return *path;
}

std::shared_ptr<std::list<std::shared_ptr<Dungeon::Block>>> Dungeon::FindPath(std::shared_ptr<Block> from, std::shared_ptr<Block> to, std::set<int>& visit)
{
	if (from->id == to->id)
	{
		return std::make_shared<std::list<std::shared_ptr<Dungeon::Block>>>();
	}

	visit.insert(from->id);

	for(auto neighbor : from->neighbors)
	{
		if (visit.end() != visit.find(neighbor->id))
		{
			continue;
		}
		std::shared_ptr<std::list<std::shared_ptr<Dungeon::Block>>> path = FindPath(neighbor, to, visit);
		if (nullptr != path)
		{
			path->push_front(neighbor);
			return path;
		}
	}

	return nullptr;
}

void Dungeon::FindDoor(std::shared_ptr<Block> block, std::shared_ptr<Block> neighbor)
{
	std::vector<RectInt> roomOffsets = {
		RectInt( 1,  1,  0, -2),    // right, ignore top, bottom tiles
		RectInt( 1, -1, -2,  0),    // down, ignore left, right tiles
		RectInt(-1,  1,  0, -2),    // left, ignore top, bottom tiels
		RectInt( 1,  1, -2,  0)     // up, ignore left, right tiles
	};

	std::vector<RectInt> corridorOffsets = {
		RectInt( 1,  0,  0,  0),	// right, ignore top, bottom tiles
		RectInt( 0, -1,  0,  0),	// down, ignore left, right tiles
		RectInt(-1,  0,  0,  0),	// left, ignore top, bottom tiels
		RectInt( 0,  1,  0,  0)		// up, ignore left, right tiles
	};

	std::vector<RectInt> offsets = roomOffsets;
	if (Block::Type::Corridor == block->type)
	{
		offsets = corridorOffsets;
	}

	std::vector<int> doorIndiceOfNeighbor = {
		-1,
		rect.width,
		1,
		-rect.width
	};

	for (int i = 0; i < (int)offsets.size(); i++)
	{
		RectInt offset = offsets[i];
		RectInt blockRect(block->rect.x + offset.x, block->rect.y + offset.y, block->rect.width + offset.width, block->rect.height + offset.height);
		std::vector<int> overlapPoints = GetIntersectArea(blockRect, neighbor->rect);
		if (0 < overlapPoints.size())
		{
			for(int overlapPoint : overlapPoints)
			{
				if (Tile::Type::Door == tiles[overlapPoint]->type)
				{
					return;
				}
			}

			int door = overlapPoints[Gamnet::Random::Range(0, (int32_t)overlapPoints.size() - 1)];
			tiles[door]->type = Tile::Type::Door;
			tiles[door + doorIndiceOfNeighbor[i]]->type = Tile::Type::Door;
			block->doors.push_back(door + doorIndiceOfNeighbor[i]);
			neighbor->doors.push_back(door);
			return;
		}
	}
}
void Dungeon::MoveToEmptySpace(std::shared_ptr<Block> block)
{
	float x = Gamnet::Random::Range(1.0f, (float)block->rect.width - 1);
	float y = Gamnet::Random::Range(1.0f, (float)block->rect.height - 1);

	int dx = 1;
	if (0 == Gamnet::Random::Range(0, 1))
	{
		dx = -1;
	}

	int dy = 1;
	if (0 == Gamnet::Random::Range(0, 1))
	{
		dy = -1;
	}

	while (true)
	{
		if (true == OverlapWithExistBlocks(block))
		{
			if (1.0f > y / x)
			{
				block->rect.x += dx * min_distance;
				block->rect.y = dy * (int)(y / x * block->rect.x) + Gamnet::Random::Range(0, 3);
			}
			else
			{
				block->rect.y += dy * min_distance;
				block->rect.x = dx * (int)(x / y * block->rect.y) + Gamnet::Random::Range(0, 3);
			}
		}
		else
		{
			break;
		}
	}
}

bool Dungeon::OverlapWithExistBlocks(std::shared_ptr<Block> block)
{
	for(std::shared_ptr<Block> exist : blocks)
	{
		if (true == exist->rect.Overlaps(block->rect))
		{
			return true;
		}
	}
	return false;
}

std::shared_ptr<Dungeon::Block> Dungeon::FindBlock(int blockID)
{
	for(auto block : blocks)
	{
		if(blockID == block->id)
		{
			return block;
		}
	}
	return nullptr;
}

std::list<std::shared_ptr<Dungeon::Block>> Dungeon::FindNeighborBlocks(int blockID)
{
	std::list<std::shared_ptr<Block>> neighbors;
   
	std::vector<RectInt> roomOffsets = {
		RectInt( 1,  1,  0, -2),	// right, ignore top, bottom tiles
		RectInt( 1, -1, -2,  0),	// down, ignore left, right tiles
		RectInt(-1,  1,  0, -2),	// left, ignore top, bottom tiels
		RectInt( 1,  1, -2,  0)		// up, ignore left, right tiles
	};

	std::vector<RectInt> corridorOffsets = {
		RectInt( 1,  0,  0,  0),	// right, ignore top, bottom tiles
		RectInt( 0, -1,  0,  0),	// down, ignore left, right tiles
		RectInt(-1,  0,  0,  0),	// left, ignore top, bottom tiels
		RectInt( 0,  1,  0,  0)		// up, ignore left, right tiles
	};

	std::shared_ptr<Block> block = FindBlock(blockID);
	std::vector<RectInt> offsets = roomOffsets;
	if (Block::Type::Corridor == block->type)
	{
		offsets = corridorOffsets;
	}

	for(auto neighbor : blocks)
	{
		if (block->id == neighbor->id)
		{
			continue;
		}

		for (int i = 0; i < (int)offsets.size(); i++)
		{
			const RectInt& offset = offsets[i];
			RectInt blockRect(block->rect.x + offset.x, block->rect.y + offset.y, block->rect.width + offset.width, block->rect.height + offset.height);
			std::vector<int> overlapPoints = GetIntersectArea(blockRect, neighbor->rect);
			if (0 < overlapPoints.size())
			{
				neighbors.push_back(neighbor);
			}
		}
	}
	return neighbors;
}

std::vector<int> Dungeon::GetIntersectArea(const RectInt& lhs, const RectInt& rhs)
{
	std::vector<int> overlapPoints;

	if(false == lhs.Overlaps(rhs))
	{
		return overlapPoints;
	}

	RectInt intersection;
	intersection.x = std::max(lhs.x, rhs.x);
	intersection.y = std::max(lhs.y, rhs.y);
	intersection.xMax = std::min((int)lhs.xMax, (int)rhs.xMax);
	intersection.yMax = std::min((int)lhs.yMax, (int)rhs.yMax);

	for (int x = intersection.x; x < intersection.xMax; x++)
	{
		for (int y = intersection.y; y < intersection.yMax; y++)
		{
			if (Tile::Type::Invalid != tiles[y * rect.width + x]->type)
			{
				overlapPoints.push_back(y * rect.width + x);
			}
		}
	}
	return overlapPoints;
}

void Dungeon::BuildWall()
{
	for(auto tile : tiles)
	{
		tile->type = Tile::Type::Invalid;
	}

	for(auto block : blocks)
	{
		if (Block::Type::Room == block->type)
		{
			BuildRoomInBlock(block);
		}

		if (Block::Type::Corridor == block->type)
		{
			BuildCorridorInBlock(block);
		}
	}
}

void Dungeon::BuildRoomInBlock(std::shared_ptr<Block> block)
{
	for (int x = block->rect.x; x < block->rect.xMax; x++)
	{
		tiles[block->rect.y * rect.width + x]->type = Tile::Type::Wall;
		tiles[(block->rect.yMax - 1) * rect.width + x]->type = Tile::Type::Wall;
	}

	for (int y = block->rect.y; y < block->rect.yMax; y++)
	{
		tiles[y * rect.width + block->rect.x]->type = Tile::Type::Wall;
		tiles[y * rect.width + block->rect.xMax - 1]->type = Tile::Type::Wall;
	}

	for (int y = block->rect.y + 1; y < block->rect.yMax - 1; y++)
	{
		for (int x = block->rect.x + 1; x < block->rect.xMax - 1; x++)
		{
			tiles[y * rect.width + x]->type = Tile::Type::Floor;
		}
	}

	for(int door : block->doors)
	{
		tiles[door]->type = Tile::Type::Door;
	}
}

void Dungeon::BuildCorridorInBlock(std::shared_ptr<Block> block)
{
	std::vector<Vector2Int> offsets = {
		Vector2Int(1,  0),
		Vector2Int(0, -1),
		Vector2Int(-1,  0),
		Vector2Int(0,  1)
	};

	std::list<int> corridor;
	if (2 <= block->doors.size())
	{
		int start = block->doors[0];

		for (size_t i = 1; i < block->doors.size(); i++)
		{
			int dest = block->doors[i];

			std::vector<std::shared_ptr<Path>> queue;
			std::set<int> visit;
			
			queue.push_back(std::make_shared<Path>(0, start));
			visit.insert(start);

			int queueIndex = 0;
			while (queueIndex < (int)queue.size())
			{
				std::shared_ptr<Path> path = queue[queueIndex++];
				if (dest == path->tile_index)
				{
					break;
				}

				int x = path->tile_index % rect.width;
				int y = path->tile_index / rect.width;

				for(Vector2Int offset : offsets)
				{
					int dx = x + offset.x;
					int dy = y + offset.y;
					if (visit.end() == visit.find(dy * rect.width + dx) && block->rect.x <= dx && dx < block->rect.xMax && block->rect.y <= dy && dy < block->rect.yMax)
					{
						queue.push_back(std::make_shared<Path>(path->tile_index, dy * rect.width + dx ));
						visit.insert(dy * rect.width + dx);
					}
				}
			}

			int prev = dest;
			for (int j = (int)queue.size() - 1; j >= 0; j--)
			{
				std::shared_ptr<Path> path = queue[j];
				if (prev == path->tile_index)
				{
					corridor.push_back(path->tile_index);
					prev = path->prev;
				}
			}
		}
	}

	std::vector<int> surroundTileOffsets = {
		rect.width - 1,
		rect.width,
		rect.width + 1,
		1,
		-(rect.width + 1),
		-rect.width,
		-(rect.width - 1),
		-1
	};

	for(int tileIndex : corridor)
	{
		tiles[tileIndex]->type = Tile::Type::Floor;

		for (int i = 0; i < (int)surroundTileOffsets.size(); i++)
		{
			int surroundTileIndex = tileIndex + surroundTileOffsets[i];
			if (0 <= surroundTileIndex && surroundTileIndex < rect.width * rect.height)
			{
				if (Tile::Type::Invalid == tiles[surroundTileIndex]->type)
				{
					tiles[surroundTileIndex]->type = Tile::Type::Wall;
				}
			}
		}
	}
}
Dungeon::Block::Block(int id)
	: id(id)
	, type(Type::Invalid)
	, distance(0)
{
}

Dungeon::Path::Path(int prev, int tileIndex)
	: prev(prev)
	, tile_index(tileIndex)
{
}
}
