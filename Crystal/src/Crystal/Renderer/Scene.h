#pragma once
#include "Crystal/GamePlay/Components/LightComponent.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"

namespace Crystal {

#define TO_TEXT(type) #type
	
	
	struct Counter
	{
		static uint64_t CreateNewId()
		{
			static uint64_t id;
			return id++;
		}
	};

	struct Scene
	{
		std::vector<std::shared_ptr<Object>> ObjectIdToSubObjects; // Key : ObjectID
		std::unordered_map<std::string, std::vector<std::shared_ptr<Object>>> ObjectTypeToObjects; /// Key : ObjectType

		template<typename T>
		void Add()
		{
			
		}
	};

}
