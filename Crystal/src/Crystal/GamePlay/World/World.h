#pragma once
#include <vector>

#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Objects/Actors/LineActor.h"
#include "PhysicsWorld.h"

namespace Crystal {
	

	class Level : public Updatable
	{
	public:
		enum LevelOwnerType
		{
			Owner_World = 0
		};
	public:
		Level()
		{
			m_PhysicsWorld = std::make_unique<PhysicsWorld>();
			m_PhysicsWorld->SetObjectName("LevelPhysicsWorld");
			m_PhysicsWorld->OnCreate();
		}
		~Level() override = default;

		void Update(const float deltaTime) override
		{
			Updatable::Update(deltaTime);
			for (const auto& actor : m_Actors)
			{
				actor->Update(deltaTime);
				actor->UpdateComponents(deltaTime);
			}

			m_PhysicsWorld->Update(deltaTime);
		}

		template<class T>
		T* SpawnActor(const std::string& name = "")
		{
			// Create new actor
			auto newActor = std::make_shared<T>();
			newActor->SetObjectOwner(weak_from_this(), Actor::ActorOwnerType::Owner_Level);
			newActor->OnCreate();
			if(!name.empty())
				newActor->SetObjectName(name);
			newActor->Begin();

			auto rawReturnActor = newActor.get(); // Get raw pointer before move
			m_Actors.push_back(std::move(newActor));

			return rawReturnActor;
		}
		//void RemoveActor() {}

		void DrawDebugLine(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
			const DirectX::XMFLOAT3& color = { 0.0f, 1.0f, 0.0f })
		{
			LineActor* debugLineActor = SpawnActor<LineActor>();
			auto lineComponent = debugLineActor->GetLineComponent();

			const auto endSubStart = Vector3::Subtract(endPoint, startPoint);

			const auto direction = Vector3::Normalize(endSubStart);
			const auto maxDistance = Vector3::Length(endSubStart);

			lineComponent->SetOrigin(startPoint);
			lineComponent->SetDirection(direction);
			lineComponent->SetMaxDistance(maxDistance);
			lineComponent->SetLineColor(color);
		}

		void DrawDebugLine(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float maxDistance,
			const DirectX::XMFLOAT3& color = { 0.0f, 1.0f, 0.0f })
		{
			LineActor* debugLineActor = SpawnActor<LineActor>();
			auto lineComponent = debugLineActor->GetLineComponent();
			lineComponent->SetOrigin(origin);
			lineComponent->SetDirection(direction);
			lineComponent->SetMaxDistance(maxDistance);
			lineComponent->SetLineColor(color);
		}

		void RegisterPhysicsWorldComponent(std::weak_ptr<Component> component)
		{			
			m_PhysicsWorld->RegisterPhysicsWorldComponent(component);
		}
		STATIC_TYPE_IMPLE(Level)
	private:
		std::unique_ptr<PhysicsWorld> m_PhysicsWorld = nullptr;
		std::vector<std::shared_ptr<Actor>> m_Actors;
	};

	class World : public Updatable
	{
	public:
		World() = default;
		~World() override = default;

		template<class T>
		T* SpawnActor(const std::string& name, Level* level = nullptr)
		{
			//std::find(new );

			// Need some validation like type checking...
			if (level)
			{
				return level->SpawnActor<T>(name);
			}

			return m_Levels[0]->SpawnActor<T>(name);
		}

		Level* CreateNewLevel(const std::string& name = "")
		{
			auto level = std::make_shared<Level>();
			level->OnCreate();
			if(!name.empty())
				level->SetObjectName(name);
			level->SetObjectOwner(weak_from_this(), Level::LevelOwnerType::Owner_World);

			m_Levels.push_back(std::move(level));

			return m_Levels.back().get();
		}

		Level* GetLevelByName(const std::string& name)
		{
			auto it = std::find_if(m_Levels.begin(), m_Levels.end(), [name](const std::shared_ptr<Level>& level)->bool
				{
					return level->GetObjectName() == name;
				}
			);

			if (it == m_Levels.end())
			{
				CS_WARN("%s 이름의 Level을 찾을 수 없습니다.", name.c_str());
				return nullptr;
			}

			return (*it).get();
		}

		void Update(const float deltaTime) override
		{
			Updatable::Update(deltaTime);

			for (const auto& level : m_Levels)
				level->Update(deltaTime);
		}

		STATIC_TYPE_IMPLE(World)
	private:
		std::vector<std::shared_ptr<Level>> m_Levels;
	};
}
