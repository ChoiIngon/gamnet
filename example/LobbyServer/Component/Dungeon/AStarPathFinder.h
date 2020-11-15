#ifndef _ASTAR_PATH_FINDER_H_
#define _ASTAR_PATH_FINDER_H_

#include "Vector2.h"
#include "Rect.h"
#include <memory>
#include "Dungeon.h"

class AStarPathFinder
{
	struct Node : public std::enable_shared_from_this<Node>
	{
		Node(AStarPathFinder* pathFinder, std::shared_ptr<Node> parent, const Vector2Int& from, int pathCost);
		int GetCost() const;
		void FindPath();

		AStarPathFinder* const path_finder;
		std::shared_ptr<Node> parent;
		Vector2Int position;
		int path_cost;
		int expect_cost;
		int index;

		static const Vector2Int LOOKUP_OFFSETS[4];
	};

public :
	AStarPathFinder(std::shared_ptr<Component::Dungeon::Data>& dungeon, const Vector2Int& from, const Vector2Int& to);
	
	std::shared_ptr<Component::Dungeon::Data> dungeon;
	std::map<int, std::shared_ptr<Node>> open_nodes;
	std::map<int, std::shared_ptr<Node>> close_nodes;
	Vector2Int destination;
	std::list<Vector2Int> path;
};

#endif