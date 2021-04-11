#pragma once
#include <vector>

#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Objects/Actors/LineActor.h"
#include "PhysicsWorld.h"

namespace Crystal {
	
	class World : public Updatable
	{
	public:
		World() = default;
		~World() override = default;

		void OnCreate() override;


		template<class T>
		T* SpawnActor(const std::string& name, Level* level = nullptr);

		Level* CreateNewLevel(const std::string& name = "");

		Level* GetLevelByName(const std::string& name);

		void Update(const float deltaTime) override;

		STATIC_TYPE_IMPLE(World)
	private:
		std::vector<std::shared_ptr<Level>> m_Levels;
	};

	template<class T>
	T* Crystal::World::SpawnActor(const std::string& name, Level* level /*= nullptr*/)
	{
		if (level)
		{
			return level->SpawnActor<T>(name);
		}

		return m_Levels[0]->SpawnActor<T>(name);
	}
}
