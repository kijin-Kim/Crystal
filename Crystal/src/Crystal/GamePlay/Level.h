#pragma once
#include <vector>
#include "World.h"

namespace Crystal {

	class Actor;

	class Level
	{
	public:
		void Update(float DeltaTime);

		void SpawnActor(Actor* actor);
		void DestroyActor(Actor* actor);

		void SetOwningWorld(World* world) { m_OwningWorld = world; }
		World* GetOwningWorld() { return m_OwningWorld; }
		
	private:
		std::vector<Actor*> m_Actors;
		World* m_OwningWorld = nullptr;
	};


}

