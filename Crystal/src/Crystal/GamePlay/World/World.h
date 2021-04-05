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

			//auto collisionShape = collisionComponent->GetWorldCollisionShape();
			//collsionShape->Instersects(otherCollisionShape);
			//collsionShape->SetWorldTransform();

			//for (const auto& obbComp1Weak : m_BoundingOrientedBoxComponents)
			//{
			//	auto obbComp1 = obbComp1Weak.lock();
			//	if(!obbComp1)
			//		continue;


			//	auto d3dOrientedBox1 = obbComp1->GetWorldBoundingOrientedBox();
			//	// OBB and Ray
			//	for (const auto& rayWeak : m_RayComponents)
			//	{
			//		auto ray = rayWeak.lock();
			//		if(!ray)
			//			continue;

			//		DirectX::XMVECTOR origin = XMLoadFloat3(&ray->GetOrigin());
			//		DirectX::XMVECTOR direction = XMLoadFloat3(&ray->GetDirection());

			//		float outDistance = 0.0f;
			//		if (d3dOrientedBox1.Intersects(origin, direction, outDistance))
			//		{
			//			if (ray->GetMaxDistance() >= abs(outDistance))
			//			{
			//				ray->SetLineColor({ 1.0f, 0.0f, 0.0f });
			//				obbComp1->SetLineColor({ 1.0f, 0.0f, 0.0f });
			//			}
			//		}
			//	}

			//	// OBB and Sphere
			//	for (const auto& sphereCompWeak : m_BoundingSphereComponents)
			//	{
			//		auto sphereComp = sphereCompWeak.lock();
			//		if(!sphereComp)
			//			continue;

			//		auto d3dBoundingSphere = sphereComp->GetWorldBoundingSphere();
			//		if (d3dOrientedBox1.Intersects(d3dBoundingSphere))
			//		{
			//			obbComp1->SetLineColor({ 1.0f, 0.0f, 0.0f });
			//			sphereComp->SetLineColor({ 1.0f, 0.0f, 0.0f });
			//		}
			//	}

			//	// OBB and AABB
			//	for (const auto& aabbCompWeak : m_BoundingBoxComponents)
			//	{
			//		auto aabbComp = aabbCompWeak.lock();
			//		if(!aabbComp)
			//			continue;

			//		auto d3dBoundingBox2 = aabbComp->GetWorldBoundingBox();
			//		if (d3dOrientedBox1.Intersects(d3dBoundingBox2))
			//		{
			//			obbComp1->SetLineColor({ 1.0f, 0.0f, 0.0f });
			//			aabbComp->SetLineColor({ 1.0f, 0.0f, 0.0f });
			//		}
			//	}

			//	// OBB and OBB
			//	for (const auto& obbComp2Weak : m_BoundingOrientedBoxComponents)
			//	{
			//		auto obbComp2 = obbComp2Weak.lock();
			//		if(!obbComp2)
			//			continue;

			//		if (obbComp1 != obbComp2)
			//		{
			//			auto d3dOrientedBox2 = obbComp2->GetWorldBoundingOrientedBox();
			//			if (d3dOrientedBox1.Intersects(d3dOrientedBox2))
			//			{
			//				obbComp1->SetLineColor({ 1.0f, 0.0f, 0.0f });
			//				obbComp2->SetLineColor({ 1.0f, 0.0f, 0.0f });
			//			}
			//		}
			//	}
			//}
			
			
			for (int i = 0; i < m_BoundingSphereComponents.size() - 1; i++)
			{
				auto sphereComplhs = Cast<BoundingSphereComponent>(m_BoundingSphereComponents[i]);
				if(!sphereComplhs)
					continue;

				auto sphereLhs = sphereComplhs->GetWorldBoundingSphere();


				for (int j = i + 1; j < m_BoundingSphereComponents.size() - i; j++)
				{
					auto sphereCompRhs = Cast<BoundingSphereComponent>(m_BoundingSphereComponents[j]);
					if(!sphereCompRhs)
						continue;

					auto sphereRhs = sphereCompRhs->GetWorldBoundingSphere();
					
					float totalDist = 0.0f;
					if (sphereLhs.Intersects(sphereRhs, totalDist))
					{
						sphereComplhs->SetLineColor(Vector3::Red);
						sphereCompRhs->SetLineColor(Vector3::Red);

						auto contactNormal = Vector3::Normalize(Vector3::Subtract(sphereComplhs->GetWorldPosition(),
									sphereCompRhs->GetWorldPosition()));


						// 분리속도 기반 충격량
						{
							auto relativeVelocity = Vector3::Subtract(sphereComplhs->GetVelocity(), sphereCompRhs->GetVelocity());
							auto contactNormal = Vector3::Normalize(Vector3::Subtract(sphereComplhs->GetWorldPosition(), 
								sphereCompRhs->GetWorldPosition()));
							auto seperatingVelocity = Vector3::Dot(relativeVelocity, contactNormal);
							if (seperatingVelocity > 0)
								continue;

							
							float newSepVelocity = -seperatingVelocity * 0.3f; // 충돌후 분리속도 = -충돌전 분리속도 * 반발계수
							float deltaVelocity = newSepVelocity - seperatingVelocity; // 속도 변화량


							float totalInverseMass = sphereComplhs->GetInverseMass() + sphereCompRhs->GetInverseMass();
							if(totalInverseMass <= 0)
								continue;

							float impulse = deltaVelocity / totalInverseMass;

							auto impulsePerIMass = Vector3::Multiply(contactNormal, impulse);

							sphereComplhs->SetVelocity(Vector3::Add(sphereComplhs->GetVelocity(), Vector3::Multiply(impulsePerIMass,
								sphereComplhs->GetInverseMass())));

							sphereCompRhs->SetVelocity(Vector3::Add(sphereCompRhs->GetVelocity(), Vector3::Multiply(impulsePerIMass,
								-sphereCompRhs->GetInverseMass())));

						}

						// 관통 해소
						{
							
							float totalInverseMass = sphereComplhs->GetInverseMass() + sphereCompRhs->GetInverseMass();
							if(totalInverseMass <= 0.0f)
								continue;
							
							auto distPerIMass = Vector3::Multiply(contactNormal, totalDist / totalInverseMass);

							auto lhsDist = Vector3::Multiply(distPerIMass, sphereComplhs->GetInverseMass());
							auto rhsDist = Vector3::Multiply(distPerIMass, -sphereCompRhs->GetInverseMass());

							CS_DEBUG_INFO("%f", totalDist);
							sphereComplhs->SetLocalPosition(Vector3::Add(sphereComplhs->GetLocalPosition(), lhsDist));
							sphereCompRhs->SetLocalPosition(Vector3::Add(sphereCompRhs->GetLocalPosition(), rhsDist));

							
						}
						
					}
				}
			}

			//for (const auto& componentWeak : m_BoundingSphereComponents)
			//{
			//	auto component = componentWeak.lock();
			//	if(!component)
			//		continue;

			//	auto d3dBoundingSphere = component->GetWorldBoundingSphere();

			//	// Bounding Sphere and Ray
			//	for (const auto& rayWeak : m_RayComponents)
			//	{
			//		auto ray = rayWeak.lock();
			//		if(!ray)
			//			continue;

			//		DirectX::XMVECTOR origin = XMLoadFloat3(&ray->GetOrigin());
			//		DirectX::XMVECTOR direction = XMLoadFloat3(&ray->GetDirection());

			//		float outDistance = 0.0f;
			//		if (d3dBoundingSphere.Intersects(origin, direction, outDistance))
			//		{
			//			if (ray->GetMaxDistance() >= abs(outDistance))
			//			{
			//				ray->SetLineColor({ 1.0f, 0.0f, 0.0f });
			//				component->SetLineColor({ 1.0f, 0.0f, 0.0f });
			//			}
			//		}
			//	}

			//	for (const auto& sphereWeak : m_BoundingSphereComponents)
			//	{
			//		auto sphereComp = sphereWeak.lock();
			//		if(!sphereComp || sphereComp == component)
			//			continue;

			//		auto d3dBoundingSphere2 = sphereComp->GetWorldBoundingSphere();
			//		if (d3dBoundingSphere.Intersects(d3dBoundingSphere2))
			//		{
			//			component->SetLineColor(Vector3::Red);
			//			sphereComp->SetLineColor(Vector3::Red);

			//			component->SetVelocity({ 0.0f, 0.0f, 0.0f });					
			//		}
			//	}

			//	
			//}
		}

		void RegisterPhysicsSystemComponent(std::weak_ptr<Component> compWeak)
		{
			auto component = compWeak.lock();
			if (!component)
				return;

			auto staticType = component->StaticType();

			if (staticType == "RayComponent")
			{
				m_RayComponents.push_back(Cast<RayComponent>(component));
			}
			else if (staticType == "BoundingBoxComponent")
			{
				m_BoundingBoxComponents.push_back(Cast<BoundingBoxComponent>(component));
			}
			else if (staticType == "BoundingOrientedBoxComponent")
			{
				m_BoundingOrientedBoxComponents.push_back(Cast<BoundingOrientedBoxComponent>(component));
			}
			else if (staticType == "BoundingSphereComponent")
			{
				m_BoundingSphereComponents.push_back(Cast<BoundingSphereComponent>(component));
			}
		}


		STATIC_TYPE_IMPLE(PhysicsSystem)
	private:
		// Actor Has OwnerShip
		std::vector<std::weak_ptr<RayComponent>> m_RayComponents;
		std::vector<std::weak_ptr<BoundingBoxComponent>> m_BoundingBoxComponents;
		std::vector<std::weak_ptr<BoundingOrientedBoxComponent>> m_BoundingOrientedBoxComponents;
		std::vector<std::weak_ptr<BoundingSphereComponent>> m_BoundingSphereComponents;
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

		void RegisterPhysicsSystemComponent(std::weak_ptr<Component> component)
		{			
			m_PhysicsSystem->RegisterPhysicsSystemComponent(component);
		}
		STATIC_TYPE_IMPLE(Level)
	private:
		std::unique_ptr<PhysicsSystem> m_PhysicsSystem = nullptr;
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
