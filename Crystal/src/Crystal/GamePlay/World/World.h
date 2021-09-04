#pragma once
#include <stack>
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
			level->Begin();
			m_Levels.push_back(level);
			return Cast<T>(m_Levels.back());
		}

		const Shared<Level>& GetLevelByName(const std::string& name);
		const Shared<Level>& GetLevelByIndex(int index);
		const Shared<Level>& GetCurrentLevel() const;

		void PushLevel(const std::string& name);
		void PopLevel(const std::string& name);

		const WorldConfig& GetWorldConfig() const { return m_WorldConfig; }
		void SetShowDebugCollision(bool show) { m_WorldConfig.bShowDebugCollision = show; }
		void SetShowDebugAI(bool show) { m_WorldConfig.bShowDebugAI = show; }


		bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		const Shared<RenderSystem>& GetRenderSystem() const { return m_RenderSystem; }
		const Shared<PhysicsSystem>& GetPhysicsSystem() const { return m_PhysicsSystem; }
		const std::vector<std::shared_ptr<Level>>& GetLevels() const { return m_Levels; }

		STATIC_TYPE_IMPLE(World)

	private:
		std::vector<std::shared_ptr<Level>> m_Levels;
		std::stack<std::shared_ptr<Level>> m_LevelStack;
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

		if(!m_LevelStack.empty())
		{
			return m_LevelStack.top()->SpawnActor<T>(spawnParams);
		}

		CS_FATAL(false, "먼저 Level을 설정해주세요");
		return {};
	}


}
