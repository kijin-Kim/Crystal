#pragma once
#include "Crystal/GamePlay/Objects/Object.h"
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
		Level();
		~Level() override = default;

		void Update(const float deltaTime) override;

		template<class T>
		T* SpawnActor(const std::string& name = "");
		//void RemoveActor() {}

		void DrawDebugLine(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
			const DirectX::XMFLOAT3& color = { 0.0f, 1.0f, 0.0f });

		void DrawDebugLine(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float maxDistance,
			const DirectX::XMFLOAT3& color = { 0.0f, 1.0f, 0.0f });

		void RegisterPhysicsWorldComponent(std::weak_ptr<Component> component);
		STATIC_TYPE_IMPLE(Level)
	private:
		std::unique_ptr<PhysicsWorld> m_PhysicsWorld = nullptr;
		std::vector<std::shared_ptr<Actor>> m_Actors;
	};

	template<class T>
	T* Crystal::Level::SpawnActor(const std::string& name /*= ""*/)
	{
		// Create new actor
		auto newActor = std::make_shared<T>();
		newActor->SetObjectOwner(weak_from_this(), Actor::ActorOwnerType::Owner_Level);
		newActor->OnCreate();
		if (!name.empty())
			newActor->SetObjectName(name);
		newActor->Begin();

		auto rawReturnActor = newActor.get(); // Get raw pointer before move
		m_Actors.push_back(std::move(newActor));

		return rawReturnActor;
	}

}
