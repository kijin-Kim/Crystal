#pragma once
#include <vector>

#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Objects/Actors/LineActor.h"

namespace Crystal {
	class PhysicsSystem : public Updatable
	{
	public:
		PhysicsSystem() = default;
		~PhysicsSystem() override = default;

		void Update(const float deltaTime) override
		{
			Updatable::Update(deltaTime);

			for (const auto obbComp1 : m_BoundingOrientedBoxComponents)
			{
				auto d3dOrientedBox1 = obbComp1->GetWorldBoundingOrientedBox();
				// OBB and Ray
				for (const auto ray : m_RayComponents)
				{
					DirectX::XMVECTOR origin = XMLoadFloat3(&ray->GetOrigin());
					DirectX::XMVECTOR direction = XMLoadFloat3(&ray->GetDirection());

					float outDistance = 0.0f;
					if (d3dOrientedBox1.Intersects(origin, direction, outDistance))
					{
						if (ray->GetMaxDistance() >= abs(outDistance))
						{
							ray->SetLineColor({ 1.0f, 0.0f, 0.0f });
							obbComp1->SetLineColor({ 1.0f, 0.0f, 0.0f });
						}
					}
				}

				// OBB and Sphere
				for (const auto sphereComp : m_BoundingSphereComponents)
				{
					auto d3dBoundingSphere = sphereComp->GetWorldBoundingSphere();
					if (d3dOrientedBox1.Intersects(d3dBoundingSphere))
					{
						obbComp1->SetLineColor({ 1.0f, 0.0f, 0.0f });
						sphereComp->SetLineColor({ 1.0f, 0.0f, 0.0f });
					}
				}

				// OBB and AABB
				for (const auto aabbComp : m_BoundingBoxComponents)
				{
					auto d3dBoundingBox2 = aabbComp->GetWorldBoundingBox();
					if (d3dOrientedBox1.Intersects(d3dBoundingBox2))
					{
						obbComp1->SetLineColor({ 1.0f, 0.0f, 0.0f });
						aabbComp->SetLineColor({ 1.0f, 0.0f, 0.0f });
					}
				}

				// OBB and OBB
				for (const auto obbComp2 : m_BoundingOrientedBoxComponents)
				{
					if (obbComp1 != obbComp2)
					{
						auto d3dOrientedBox2 = obbComp2->GetWorldBoundingOrientedBox();
						if (d3dOrientedBox1.Intersects(d3dOrientedBox2))
						{
							obbComp1->SetLineColor({ 1.0f, 0.0f, 0.0f });
							obbComp2->SetLineColor({ 1.0f, 0.0f, 0.0f });
						}
					}
				}
			}

			// Bounding Sphere and Ray
			for (const auto component : m_BoundingSphereComponents)
			{
				auto d3dBoundingSphere = component->GetWorldBoundingSphere();

				for (const auto ray : m_RayComponents)
				{
					DirectX::XMVECTOR origin = XMLoadFloat3(&ray->GetOrigin());
					DirectX::XMVECTOR direction = XMLoadFloat3(&ray->GetDirection());

					float outDistance = 0.0f;
					if (d3dBoundingSphere.Intersects(origin, direction, outDistance))
					{
						if (ray->GetMaxDistance() >= abs(outDistance))
						{
							ray->SetLineColor({ 1.0f, 0.0f, 0.0f });
							component->SetLineColor({ 1.0f, 0.0f, 0.0f });
						}
					}
				}
			}
		}

		void RegisterCollisionComponent(CollisionComponent* component)
		{
			auto type = component->GetPrimitiveComponentType();
			switch (type)
			{
			case PrimitiveComponent::EPrimitiveComponentType::Ray:
				m_RayComponents.push_back(static_cast<RayComponent*>(component));
				return;
			case PrimitiveComponent::EPrimitiveComponentType::BoundingBox:
				m_BoundingBoxComponents.push_back(static_cast<BoundingBoxComponent*>(component));
				return;
			case PrimitiveComponent::EPrimitiveComponentType::BoundingOrientedBox:
				m_BoundingOrientedBoxComponents.push_back(static_cast<BoundingOrientedBoxComponent*>(component));
				return;
			case PrimitiveComponent::EPrimitiveComponentType::BoundingSphere:
				m_BoundingSphereComponents.push_back(static_cast<BoundingSphereComponent*>(component));
				return;
			default:
				CS_FATAL(false, "잘못된 PrimitiveComponent Type입니다.");
				return;
			}
		}


		STATIC_TYPE_IMPLE(PhysicsSystem)
	private:
		// Actor Has OwnerShip
		std::vector<RayComponent*> m_RayComponents;
		std::vector<BoundingBoxComponent*> m_BoundingBoxComponents;
		std::vector<BoundingOrientedBoxComponent*> m_BoundingOrientedBoxComponents;
		std::vector<BoundingSphereComponent*> m_BoundingSphereComponents;
	};

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
			m_PhysicsSystem = std::make_unique<PhysicsSystem>();
			m_PhysicsSystem->SetObjectName("LevelPhysicsSystem");
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

			m_PhysicsSystem->Update(deltaTime);
		}

		template<class T>
		T* SpawnActor(const std::string& name = "")
		{
			// Create new actor
			auto newActor = std::make_shared<T>();
			newActor->OnCreate();
			newActor->SetObjectOwner(weak_from_this(), Actor::ActorOwnerType::Owner_Level);
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

		void RegisterCollisionComponent(CollisionComponent* component)
		{			
			//m_PhysicsSystem->RegisterCollisionComponent(component);
		}
		STATIC_TYPE_IMPLE(Level)
	private:
		std::unique_ptr<PhysicsSystem> m_PhysicsSystem = nullptr;
		std::vector<std::shared_ptr<Actor>> m_Actors;
	};

	class World : public Updatable
	{
	public:
		World()
		{
			//auto defaultLevel = std::make_unique<Level>();
			//defaultLevel->SetObjectName("DefaultLevel");
			//defaultLevel->SetObjectOwner(this, ObjectOwnerType::OOT_World);

			//m_Levels.push_back(std::move(defaultLevel)); //Default Level
		}

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
