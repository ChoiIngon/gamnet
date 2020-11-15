#include "AStarPathFinder.h"
#include "Dungeon.h"
#include <algorithm>

const Vector2Int AStarPathFinder::Node::LOOKUP_OFFSETS[4] =
{
	Vector2Int(-1,  0),
	Vector2Int(1,  0),
	Vector2Int(0, -1),
	Vector2Int(0,  1)
};

AStarPathFinder::Node::Node(AStarPathFinder* pathFinder, std::shared_ptr<Node> parent, const Vector2Int& from, int pathCost)
	: path_finder(pathFinder)
	, parent(parent)
	, position(from)
	, path_cost(pathCost)
	, expect_cost(0)
	, index(0)
{
	index = from.y * path_finder->dungeon->rect.width + from.x;
	expect_cost += std::abs(path_finder->destination.x - from.x);
	expect_cost += std::abs(path_finder->destination.y - from.y);
}

void AStarPathFinder::Node::FindPath()
{
	{
		auto itr = path_finder->open_nodes.find(index);
		if (path_finder->open_nodes.end() != itr)
		{
			path_finder->open_nodes.erase(itr);
		}
	}

	{
		auto itr = path_finder->close_nodes.find(index);
		if (path_finder->close_nodes.end() != itr)
		{
			return;
		}
	}

	path_finder->close_nodes.insert(std::make_pair(index, shared_from_this()));
	if (position == path_finder->destination)
	{
		std::shared_ptr<Node> trace = shared_from_this();
		while (nullptr != trace->parent)
		{
			path_finder->path.push_front(trace->position);
			trace = trace->parent;
		}
		path_finder->path.push_front(trace->position);
		return;
	}

	std::list<std::shared_ptr<Node>> children;
	for (const Vector2Int& offset : LOOKUP_OFFSETS)
	{
		Vector2Int childPosition(position.x + offset.x, position.y + offset.y);
		std::shared_ptr<Tile> tile = path_finder->dungeon->GetTile(childPosition.x, childPosition.y);
		if(nullptr == tile)
		{
			continue;
		}
		
		if (Tile::Type::Wall == tile->type)
		{
			continue;
		}

		std::shared_ptr<Node> childNode = std::make_shared<Node>(path_finder, shared_from_this(), childPosition, path_cost + 1); 
		{
			auto itr = path_finder->close_nodes.find(childNode->index);
			if (path_finder->close_nodes.end() != itr)
			{
				//Debug.Log ("already close node(child position:x" + childPosition.x + ", y:" + childPosition.y +")");
				continue;
			}
		}
		{
			auto itr = path_finder->open_nodes.find(childNode->index);
			if (path_finder->open_nodes.end() != itr)
			{
				std::shared_ptr<Node> openNode = itr->second;
				if (openNode->path_cost < this->path_cost)
				{
					this->path_cost = openNode->path_cost + 1;
					this->parent = openNode;
					continue;
				}
			}
			else
			{
				path_finder->open_nodes.insert(std::make_pair(childNode->index, childNode));
			}
		}
		children.push_back(childNode);
	}
	children.sort([](const std::shared_ptr<Node>& lhs, const std::shared_ptr<Node>& rhs)
	{
		return lhs->GetCost() < rhs->GetCost();
	});

	for(std::shared_ptr<Node>& child : children)
	{
		child->FindPath();
		if (0 < path_finder->path.size())
		{
			return;
		}
	}
	//Debug.Log ("cant find destination(x:" + position.x + ", y:" + position.y + ")");
}

int AStarPathFinder::Node::GetCost() const
{
	return path_cost + expect_cost;
}


AStarPathFinder::AStarPathFinder(std::shared_ptr<Component::Dungeon::Data>& dungeon, const Vector2Int& from, const Vector2Int& to)
	: dungeon(dungeon)
	, destination(to)
{
	std::shared_ptr<Node> node = std::make_shared<Node>(this, nullptr, from, 0);
	node->FindPath();
}