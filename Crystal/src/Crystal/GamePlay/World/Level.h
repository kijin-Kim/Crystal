#pragma once
#include <functional>

#include "Crystal/GamePlay/Objects/Object.h"
#include "PhysicsSystem.h"
#include "World.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/Renderer/RenderSystem.h"
#include "Crystal/Renderer/Scene.h"


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
		std::weak_ptr<T> SpawnActor(const Actor::ActorSpawnParams& spawnParams);


		void AddActor(const std::shared_ptr<Actor>& actor);
		void AddPendingSpawnedActors();
		void AddPlayerController(const std::shared_ptr<PlayerController>& playerController);

		void RemoveActor(const std::shared_ptr<Actor>& actor);
		void RemovePendingActors();

		void DrawDebugLine(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
		                   const DirectX::XMFLOAT3& color = {0.0f, 1.0f, 0.0f});

		void DrawDebugLine(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float maxDistance,
		                   const DirectX::XMFLOAT3& color = {0.0f, 1.0f, 0.0f});

		void RegisterPhysicsWorldComponent(std::weak_ptr<Component> component);


		void OnClientConnect();

		const Shared<Scene>& GetScene() { return m_Scene; }


		std::weak_ptr<Actor> GetActorByName(const std::string& name);
		std::weak_ptr<Actor> GetActorByClass(const std::string& classType);
		std::vector<std::weak_ptr<Actor>> GetAllActorByClass(const std::string& classType);

		std::weak_ptr<PlayerController> GetPlayerController(int index);
		std::weak_ptr<PlayerController> GetPlayerControllerByNetworkId(int id);

		Weak<Pawn> GetPlayerPawn() const { return m_Player; }
		Weak<Actor> GetHUD() const { return m_HUD; }


		bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

		bool LineTraceSingle(struct HitResult& outHitResult, const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float,
		                     const struct CollisionParams& collisionParams);

		STATIC_TYPE_IMPLE(Level)

	protected:
		std::vector<std::shared_ptr<Actor>>::iterator FindActorItByDelegate(
			const std::function<bool(const std::shared_ptr<Actor>&)>& delegate);


	private:
		Shared<PhysicsSystem> m_PhysicsSystem = nullptr;
		Shared<Scene> m_Scene = nullptr;
		Shared<RenderSystem> m_RenderSystem = nullptr;


		Shared<Pawn> m_Player = nullptr;
		Shared<Actor> m_HUD = nullptr;

		std::vector<std::shared_ptr<Actor>> m_Actors;
		std::vector<std::shared_ptr<Actor>> m_PendingSpawnedActors;
		std::vector<std::shared_ptr<PlayerController>> m_PlayerControllers;


		bool m_bHasDeadActors = false;
	};

	template <class T>
	std::weak_ptr<T> Crystal::Level::SpawnActor(const Actor::ActorSpawnParams& spawnParams)
	{
		// Create new actor
		auto newActor = Cast<Actor>(CreateObject<T>(spawnParams.Name, weak_from_this()));

		// Set Initial Transform
		if (!Vector3::IsZero(spawnParams.Position))
		{
			newActor->SetPosition(spawnParams.Position);
		}

		if (!Vector4::IsZero(spawnParams.Rotation))
		{
			newActor->SetRotationQuat(spawnParams.Rotation);
		}

		if (spawnParams.Scale != 0.0f)
		{
			newActor->SetUnitScale(spawnParams.Scale);
		}


		newActor->Begin();

		return Cast<T>(newActor);
	}


}
