#pragma once
#include "Crystal/GamePlay/World/World.h"

namespace Crystal {

	class ActorClassBase
	{
	public:
		virtual Actor* SpawnActor(World* world, const Actor::ActorSpawnParams& spawnParams)
		{
			return nullptr;
		}
	};
	
	// Actor Class Type Holder
	template<class T>
	class ActorClassOf : public ActorClassBase
	{
	public:	
		Actor* SpawnActor(World* world, const Actor::ActorSpawnParams& spawnParams) override
		{
			return world->SpawnActor<T>(spawnParams);
		}
	};
	
}
