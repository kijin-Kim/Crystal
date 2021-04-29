#pragma once
#include "Crystal/GamePlay/Objects/Object.h"
#include "PhysicsSystem.h"
#include "Crystal/Renderer/RenderSystem.h"




namespace Crystal {
	class LightComponent;

	class PlayerController;
	class RenderSystem;
	
	class Level : public Updatable
	{
	public:
		enum LevelOwnerType
		{
			Owner_World = 0
		};
	public:
		Level() = default;
		~Level() override = default;

		void OnCreate() override;

		void Update(const float deltaTime) override;

		template<class T>
		T* SpawnActor(const std::string& name = "");
		//void RemoveActor() {}

		void DrawDebugLine(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
			const DirectX::XMFLOAT3& color = { 0.0f, 1.0f, 0.0f });

		void DrawDebugLine(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float maxDistance,
			const DirectX::XMFLOAT3& color = { 0.0f, 1.0f, 0.0f });

		void RegisterPhysicsWorldComponent(std::weak_ptr<Component> component);
		void RegisterLightComponent(std::weak_ptr<LightComponent> componentWeak);
		void RegisterRendererComponent(std::weak_ptr<PrimitiveComponent> componentWeak);


		std::shared_ptr<Actor> GetActorByName(const std::string& name);
		std::shared_ptr<Actor> GetActorByClass(const std::string& classType);
		std::vector<std::shared_ptr<Actor>> GetAllActorByClass(const std::string& classType);

		STATIC_TYPE_IMPLE(Level)

		
	private:
		std::unique_ptr<PhysicsSystem> m_PhysicsSystem = nullptr;
		std::unique_ptr<RenderSystem> m_RenderSystem = nullptr;
		
		
		std::vector<std::shared_ptr<Actor>> m_Actors;
		
	};

	template<class T>
	T* Crystal::Level::SpawnActor(const std::string& name /*= ""*/)
	{
		// Create new actor
		auto newActor = std::make_shared<T>();
		newActor->SetOwner(weak_from_this());
		newActor->OnCreate();
		if (!name.empty())
			newActor->SetObjectName(name);
		newActor->Begin();

		auto rawReturnActor = newActor.get(); // Get raw pointer before move
		m_Actors.push_back(std::move(newActor));

		return rawReturnActor;
	}

}
