#include "cspch.h"
#include "Level.h"
#include "Crystal/GamePlay/Objects/Actors/LineActor.h"

namespace Crystal {

	Level::Level()
	{
		m_PhysicsWorld = std::make_unique<PhysicsWorld>();
		m_PhysicsWorld->SetObjectName("LevelPhysicsWorld");
		m_PhysicsWorld->OnCreate();
	}

	void Level::Update(const float deltaTime)
	{
		Updatable::Update(deltaTime);
		for (const auto& actor : m_Actors)
		{
			actor->Update(deltaTime);
			actor->UpdateComponents(deltaTime);
		}

		m_PhysicsWorld->Update(deltaTime);
	}

	void Level::DrawDebugLine(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float maxDistance, const DirectX::XMFLOAT3& color /*= { 0.0f, 1.0f, 0.0f }*/)
	{
		LineActor* debugLineActor = SpawnActor<LineActor>();
		auto lineComponent = debugLineActor->GetLineComponent();
		lineComponent->SetOrigin(origin);
		lineComponent->SetDirection(direction);
		lineComponent->SetMaxDistance(maxDistance);
		lineComponent->SetLineColor(color);
	}

	void Level::DrawDebugLine(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint, const DirectX::XMFLOAT3& color /*= { 0.0f, 1.0f, 0.0f }*/)
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

	void Level::RegisterPhysicsWorldComponent(std::weak_ptr<Component> component)
	{
		m_PhysicsWorld->RegisterPhysicsWorldComponent(component);
	}

}


