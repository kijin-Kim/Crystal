#pragma once
#include <vector>

#include "Crystal/GamePlay/Actors/Actor.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Actors/LineActor.h"

namespace Crystal {

	class PhysicsSystem : public Object
	{
	public:
		PhysicsSystem(Object* parent) : Object(parent) {}
		~PhysicsSystem() override = default;

		void Update(const float deltaTime) override 
		{ 
			Object::Update(deltaTime);

			
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
							//ray->SetLineColor({ 1.0f, 0.0f, 0.0f });
							//obbComp1->SetLineColor({ 1.0f, 0.0f, 0.0f });
						}
					}
				}

				// OBB and Sphere
				for (const auto sphereComp : m_BoundingSphereComponents)
				{
					auto d3dBoundingSphere = sphereComp->GetWorldBoundingSphere();
					if (d3dOrientedBox1.Intersects(d3dBoundingSphere))
					{
						//obbComp1->SetLineColor({ 1.0f, 0.0f, 0.0f });
						//sphereComp->SetLineColor({ 1.0f, 0.0f, 0.0f });
					}
				}

				// OBB and AABB
				for (const auto aabbComp : m_BoundingBoxComponents)
				{
					auto d3dBoundingBox2 = aabbComp->GetWorldBoundingBox();
					if (d3dOrientedBox1.Intersects(d3dBoundingBox2))
					{
						//obbComp1->SetLineColor({ 1.0f, 0.0f, 0.0f });
						//aabbComp->SetLineColor({ 1.0f, 0.0f, 0.0f });
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
							//obbComp1->SetLineColor({ 1.0f, 0.0f, 0.0f });
							//obbComp2->SetLineColor({ 1.0f, 0.0f, 0.0f });
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
							//ray->SetLineColor({ 1.0f, 0.0f, 0.0f });
							//component->SetLineColor({ 1.0f, 0.0f, 0.0f });
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

	private:
		// Actor Has OwnerShip
		std::vector<RayComponent*> m_RayComponents;
		std::vector<BoundingBoxComponent*> m_BoundingBoxComponents;
		std::vector<BoundingOrientedBoxComponent*> m_BoundingOrientedBoxComponents;
		std::vector<BoundingSphereComponent*> m_BoundingSphereComponents;
		

	};

	class Level : public Object
	{
	public:
		Level(Object* parent) : Object(parent)
		{
			m_PhysicsSystem = std::make_unique<PhysicsSystem>(this);
		}
		~Level() override = default;

		void Update(const float deltaTime) override
		{
			Object::Update(deltaTime);
			for (const auto& actor : m_Actors)
			{
				actor->Update(deltaTime);
				actor->UpdateComponents(deltaTime);
			}

			m_PhysicsSystem->Update(deltaTime);
		}

		template<class T>
		T* SpawnActorInLevel()
		{	
			// Create new actor
			auto newActor = std::make_unique<T>(this);
			newActor->Begin();

			auto rawReturnActor = newActor.get(); // Get raw pointer before move
			m_Actors.push_back(std::move(newActor));
			
			return rawReturnActor;
		}
		//void RemoveActor() {}

		void DrawDebugLine(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint, 
			const DirectX::XMFLOAT3& color = { 0.0f, 1.0f, 0.0f })
		{
			LineActor* debugLineActor = SpawnActorInLevel<LineActor>();
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
			LineActor* debugLineActor = SpawnActorInLevel<LineActor>();
			auto lineComponent = debugLineActor->GetLineComponent();
			lineComponent->SetOrigin(origin);
			lineComponent->SetDirection(direction);
			lineComponent->SetMaxDistance(maxDistance);
			lineComponent->SetLineColor(color);
		}

		void RegisterCollisionComponent(CollisionComponent* component) 
		{
			m_PhysicsSystem->RegisterCollisionComponent(component); 
		}
		
	private:
		std::unique_ptr<PhysicsSystem> m_PhysicsSystem = nullptr;
		std::vector<std::unique_ptr<Actor>> m_Actors;
	};


	class World : public Object
	{
	public:
		World(Object* parent) : Object(parent)
		{
			auto defaultLevel = std::make_unique<Level>(this);
			m_Levels.push_back(std::move(defaultLevel)); //Default Level
		}


		~World() override = default;

		template<class T>
		T* SpawnActor(Level* level = nullptr)
		{
			//std::find(new );

			// Need some validation like type checking...
			if (level)
			{
				return level->SpawnActorInLevel<T>();
			}
			
			return m_Levels[0]->SpawnActorInLevel<T>();
		}

		void Update(const float deltaTime) override
		{
			Object::Update(deltaTime);

			for (const auto& level : m_Levels)
				level->Update(deltaTime);
		}

	private:
		std::vector<std::unique_ptr<Level>> m_Levels;
	};


}
