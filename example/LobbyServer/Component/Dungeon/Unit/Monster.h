#ifndef _MONSTER_H_
#define _MONSTER_H_

#include <Gamnet/Library/MetaData.h>

#include "BehaviourTree.h"

struct MonsterMeta : public Gamnet::MetaData
{
	MonsterMeta();

	std::string id;
	uint32_t	index;
	std::string name;

	BehaviourTree behaviour_tree;
};


#endif
