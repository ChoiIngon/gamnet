#ifndef _MONSTER_H_
#define _MONSTER_H_


#include <Gamnet/Library/MetaData.h>

#include "BehaviourTree.h"

class Unit;
namespace Component { namespace Monster {
	class Meta : public Gamnet::MetaData
	{
	public :
		Meta();

		std::string id;
		uint32_t	index;
		std::string name;
		std::shared_ptr<BehaviourTree<Unit>> behaviour;
	private :
		void OnBehaviourPath(std::shared_ptr<BehaviourTree<Unit>>& behaviour, const std::string& value);
	};

	class Data
	{
	public :
		std::shared_ptr<Meta> meta;
	};

	class Manager
	{
	public :
		void Init();
		std::shared_ptr<Meta> FindMeta(const std::string& id);
		std::shared_ptr<Meta> FindMeta(uint32_t index);
		std::shared_ptr<Data> CreateInstance(const std::string& id);
		std::shared_ptr<Data> CreateInstance(uint32_t index);
	private :
		std::shared_ptr<Data> CreateInstance(const std::shared_ptr<Meta>& meta);
		std::map<uint32_t, std::shared_ptr<Meta>> index_metas;
		std::map<std::string, std::shared_ptr<Meta>> id_metas;
	};
}}
#endif
