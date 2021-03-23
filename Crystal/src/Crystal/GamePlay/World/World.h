#pragma once
#include <vector>

#include "Crystal/GamePlay/Actors/Actor.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Actors/LineActor.h"

namespace Crystal {
	class Level final
	{
	public:
		Level() = default;
		~Level() = default;

		void AddActor(const std::shared_ptr<Actor>& actor) 
		{ 
			actor->Begin();
			actor->SetLevel(this);
			m_Actors.push_back(actor); 
		}
		//void RemoveActor() {}

		void Update(float deltaTime)
		{
			for (const auto& actor : m_Actors)
			{
				actor->Update(deltaTime);
				actor->UpdateComponents(deltaTime);
			}
		}

		void SetWorld(World* world) { m_World = world; }
		World* GetWorld() const { return m_World; }

	private:
		std::vector<std::shared_ptr<Actor>> m_Actors;
		World* m_World = nullptr;
	};

	class World final
	{
	public:
		World()
		{
			auto defaultLevel = new Level();
			defaultLevel->SetWorld(this);
			m_Levels.push_back(defaultLevel); //Default Level
		}
		~World() = default;

		template<class T>
		T* SpawnActor(Level* level = nullptr)
		{
			// Need some validation like type checking...
			std::shared_ptr<T> newActor = std::make_shared<T>();
			if (level == nullptr)
			{
				m_Levels[0]->AddActor(newActor);
			}
			else
			{
				level->AddActor(newActor);
			}
			return newActor.get();
		}

		void Update(float deltaTime)
		{
			for (const auto level : m_Levels)
				level->Update(deltaTime);
		}

		void DrawDebugLine(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint)
		{
			LineActor* debugLineActor = SpawnActor<LineActor>();
			auto lineComponent = debugLineActor->GetLineComponent();
			lineComponent->SetStartPoint(startPoint);
			lineComponent->SetEndPoint(endPoint);
		}

	private:
		std::vector<Level*> m_Levels;
	};
}
