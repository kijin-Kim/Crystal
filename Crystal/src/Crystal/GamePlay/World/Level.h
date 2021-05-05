#pragma once
#include <functional>

#include "Crystal/GamePlay/Objects/Object.h"
#include "PhysicsSystem.h"
#include "World.h"
#include "Crystal/Renderer/RenderSystem.h"


namespace Crystal {
	class LightComponent;

	class PlayerController;
	class RenderSystem;

	class Level : public Object
	{
	public:
		Level() = default;
		~Level() override = default;

		void OnCreate() override;


		void Update(const float deltaTime) override;

		template <class T>
		std::weak_ptr<T> SpawnActor(const std::string& name = "");
		template <class T>
		T* SpawnProtoTypedActor(const std::string& prototypedActorName,
		                        const Actor::ActorSpawnParams& spawnParams);

		void AddActor(const std::shared_ptr<Actor>& actor);
		void AddPlayerController(const std::shared_ptr<PlayerController>& playerController);

		void DestroyActor(const std::shared_ptr<Actor>& actor);
		void DestroyPendingActors();

		void DrawDebugLine(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
		                   const DirectX::XMFLOAT3& color = {0.0f, 1.0f, 0.0f});

		void DrawDebugLine(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float maxDistance,
		                   const DirectX::XMFLOAT3& color = {0.0f, 1.0f, 0.0f});

		void RegisterPhysicsWorldComponent(std::weak_ptr<Component> component);
		void RegisterLightComponent(std::weak_ptr<LightComponent> componentWeak);
		void RegisterRendererComponent(std::weak_ptr<PrimitiveComponent> componentWeak);

		void OnClientConnect();


		std::weak_ptr<Actor> GetActorByName(const std::string& name);
		std::weak_ptr<Actor> GetActorByClass(const std::string& classType);
		std::vector<std::weak_ptr<Actor>> GetAllActorByClass(const std::string& classType);
		

		bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		STATIC_TYPE_IMPLE(Level)

	protected:
		std::vector<std::shared_ptr<Actor>>::iterator FindActorItByDelegate(
			const std::function<bool(const std::shared_ptr<Actor>&)>& delegate);


	private:
		std::unique_ptr<PhysicsSystem> m_PhysicsSystem = nullptr;
		std::unique_ptr<RenderSystem> m_RenderSystem = nullptr;


		std::vector<std::shared_ptr<Actor>> m_Actors;
		std::vector<std::shared_ptr<PlayerController>> m_PlayerControllers;
	};

	template <class T>
	std::weak_ptr<T> Crystal::Level::SpawnActor(const std::string& name /*= ""*/)
	{
		// Create new actor
		auto newActor = CreateObject<T>(name, weak_from_this());

		newActor->Begin();

		m_Actors.push_back(newActor);

		return newActor;
	}


	template <class T>
	T* Level::SpawnProtoTypedActor(const std::string& prototypedActorName, const Actor::ActorSpawnParams& spawnParams)
	{
		auto world = Cast<World>(GetOuter());
		auto actorPrototype = world->GetProtoTypeActor(prototypedActorName);
		if (actorPrototype)
		{
			/*	actorPrototype->SetObjectName(spawnParams.Name);
				actorPrototype->Begin();
				m_Actors.push_back(actorPrototype);*/
		}
	}


}
