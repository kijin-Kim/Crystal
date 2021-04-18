#pragma once
#include <vector>

#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "PhysicsSystem.h"

namespace Crystal {
	

	class PlayerController;


	class World : public Updatable
	{
	public:
		World() = default;
		~World() override = default;

		void OnCreate() override;

		void Update(const float deltaTime) override;


		template<class T>
		T* SpawnActor(const std::string& name = "", Level* level = nullptr);

		Level* CreateNewLevel(const std::string& name = "");

		Level* GetLevelByName(const std::string& name);
		Level* GetLevelByIndex(int index);
		Level* GetCurrentLevel();
		
		void SetCurrentLevelByName(const std::string& name);
		void SetCurrentLevelByIndex(int iIndex);


		


		STATIC_TYPE_IMPLE(World)
	private:
		Level* m_CurrentLevel = nullptr;
		std::vector<std::shared_ptr<Level>> m_Levels;
	};

	template<class T>
	T* Crystal::World::SpawnActor(const std::string& name /*= ""*/, Level* level /*= nullptr*/)
	{
		if (level)
		{
			return level->SpawnActor<T>(name);
		}

		if (m_CurrentLevel)
		{
			return m_CurrentLevel->SpawnActor<T>(name);
		}

		CS_FATAL(false, "먼저 Level을 설정해주세요");
	}
}
