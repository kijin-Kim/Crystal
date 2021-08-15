#pragma once
#include <vector>

#include "Level.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "PhysicsSystem.h"
#include "Crystal/Renderer/RenderSystem.h"


namespace Crystal {


	struct WorldConfig
	{
		bool bShowDebugCollision = false;
		bool bShowDebugAI = false;
	};


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

		template<class T>
		const Shared<T>& CreateLevel(const std::string& name = "")
		{
			auto level = CreateObject<T>(name, weak_from_this());
			m_Levels.push_back(level);
			return Cast<T>(m_Levels.back());
		}

		const Shared<Level>& GetLevelByName(const std::string& name);
		const Shared<Level>& GetLevelByIndex(int index);
		const Shared<Level>& GetCurrentLevel() const;

		void SetCurrentLevelByName(const std::string& name);
		void SetCurrentLevelByIndex(int iIndex);

		const WorldConfig& GetWorldConfig() const { return m_WorldConfig; }
		void SetShowDebugCollision(bool show) { m_WorldConfig.bShowDebugCollision = show; }
		void SetShowDebugAI(bool show) { m_WorldConfig.bShowDebugAI = show; }


		bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		const Shared<RenderSystem>& GetRenderSystem() const { return m_RenderSystem; }
		const Shared<PhysicsSystem>& GetPhysicsSystem() const { return m_PhysicsSystem; }

		STATIC_TYPE_IMPLE(World)

	private:
		Shared<Level> m_CurrentLevel = nullptr;
		std::vector<std::shared_ptr<Level>> m_Levels;
		WorldConfig m_WorldConfig = {};

		Shared<RenderSystem> m_RenderSystem = nullptr;
		Shared<PhysicsSystem> m_PhysicsSystem = nullptr;
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
