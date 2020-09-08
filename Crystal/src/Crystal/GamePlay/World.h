#pragma once
#include <memory>
#include <vector>

namespace Crystal {

	class Level;
	class Actor;

	class World
	{
	public:
		World();
		~World();

		void Update(float DeltaTime);

		void AddLevel(Level* level);
		void RemoveLevel(Level* level);

		void SpawnActor(Actor* actor, Level* level);
		void DestroyActor(Actor* actor);


	private:
		std::vector<Level*> m_Levels;
	};

}

