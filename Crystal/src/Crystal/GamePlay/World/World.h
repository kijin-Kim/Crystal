#pragma once
#include <vector>

#include "Level.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "PhysicsSystem.h"

namespace Crystal {

	class PlayerController;

	class World : public Object
	{
	public:
		World() = default;
		~World() override = default;

		void OnCreate() override;

		void Update(const float deltaTime) override;


		template <class T>
		std::weak_ptr<T> SpawnActor(const Actor::ActorSpawnParams& spawnParams);

		void DestroyActor(const std::shared_ptr<Actor>& actor);

		Level* CreateNewLevel(const std::string& name = "");

		Level* GetLevelByName(const std::string& name);
		Level* GetLevelByIndex(int index);
		Level* GetCurrentLevel();

		void SetCurrentLevelByName(const std::string& name);
		void SetCurrentLevelByIndex(int iIndex);



		bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		STATIC_TYPE_IMPLE(World)

	private:
		Level* m_CurrentLevel = nullptr;
		std::vector<std::shared_ptr<Level>> m_Levels;
	};

	template <class T>
	std::weak_ptr<T> World::SpawnActor(const Actor::ActorSpawnParams& spawnParams)
	{
		if (spawnParams.Level)
		{
			return spawnParams.Level->SpawnActor<T>(spawnParams);
		}

		if (m_CurrentLevel)
		{
			return m_CurrentLevel->SpawnActor<T>(spawnParams);
		}

		CS_FATAL(false, "먼저 Level을 설정해주세요");
		return {};
	}


}
