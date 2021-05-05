#include "cspch.h"
#include "Level.h"
#include "Crystal/GamePlay/Objects/Actors/LineActor.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"
#include "Crystal/GamePlay/Objects/Actors/ActorClassOf.h"

namespace Crystal {

	void Level::OnCreate()
	{
		Object::OnCreate();

		m_PhysicsSystem = std::make_unique<PhysicsSystem>();
		m_PhysicsSystem->Initialize();
		m_PhysicsSystem->SetObjectName("LevelPhysicsSystem");
		m_PhysicsSystem->OnCreate();

#ifndef CS_NM_DEDICATED
		m_RenderSystem = std::make_unique<RenderSystem>();
		m_RenderSystem->Initialize();
		m_RenderSystem->SetOuter(weak_from_this());
		m_RenderSystem->SetObjectName("LevelRenderSystem");
		m_RenderSystem->OnCreate();
		m_RenderSystem->Begin();
#endif
	}

	void Level::Update(const float deltaTime)
	{
		Object::Update(deltaTime);


		for (const auto& actor : m_Actors)
		{
			actor->UpdateComponents(deltaTime);
			actor->Update(deltaTime);
			if (actor->GetIsDead())
			{
				actor->End();
			}
		}
		m_PhysicsSystem->Update(deltaTime);


		//using Type = int;
		//std::unordered_map<Type, int> m_CountMap;
		//std::unordered_map<Type, int> m_CountMap2;
		////================서버
		//for(auto& m : m_CountMap)
		//{
		//	for(auto& m2 : m_CountMap2)
		//	{				
		//		if(m.second !=  m2.second)
		//		{
		//			Type->SpawnActor();
		//			Type->DestroyActor();
		//		}
		//	}
		//}


		DestroyPendingActors();


		if (m_RenderSystem)
		{
			m_RenderSystem->Update(deltaTime);
		}
	}

	void Level::DrawDebugLine(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float maxDistance,
	                          const DirectX::XMFLOAT3& color /*= { 0.0f, 1.0f, 0.0f }*/)
	{
		auto debugLineActor = SpawnActor<LineActor>().lock();
		auto lineComponent = debugLineActor->GetLineComponent();
		lineComponent->SetOrigin(origin);
		lineComponent->SetDirection(direction);
		lineComponent->SetMaxDistance(maxDistance);
		lineComponent->SetLineColor(color);
	}

	void Level::DestroyActor(const std::shared_ptr<Actor>& actor)
	{
		actor->SetIsDead(true);
	}

	void Level::DestroyPendingActors()
	{
		for (auto it = m_Actors.begin(); it != m_Actors.end();)
		{
			if (!(*it)->GetIsDead())
			{
				++it;
				continue;
			}

			it = m_Actors.erase(it);
		}
	}

	void Level::DrawDebugLine(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
	                          const DirectX::XMFLOAT3& color /*= { 0.0f, 1.0f, 0.0f }*/)
	{
		auto debugLineActor = SpawnActor<LineActor>().lock();
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
		m_PhysicsSystem->RegisterPhysicsWorldComponent(component);
	}

	void Level::RegisterLightComponent(std::weak_ptr<LightComponent> componentWeak)
	{
		if (m_RenderSystem)
		{
			m_RenderSystem->RegisterLightComponent(componentWeak);
		}
	}

	void Level::RegisterRendererComponent(std::weak_ptr<PrimitiveComponent> componentWeak)
	{
		if (m_RenderSystem)
		{
			m_RenderSystem->RegisterPrimitiveComponentNew(componentWeak);
		}
	}

	void Level::OnClientConnect()
	{
#ifdef CS_NM_CLIENT
		/* 멀티플레이어 클라이언트면,
		 * 서버가 알아서 생성
		 * return;
		 *
		 *
		 */
#endif
		
#ifdef CS_NM_STANDALONE
		/* 스탠드얼론이 면,
		 * 0. PlayerStartActor를 검색하고, 없으면 접속을 거부. Return.
		 * 1. 새로운 PlayerController를 Spawn합니다.
		 * 2. PlayerStartActor를 Destroy하고, PlayerStartActor로부터 새로운 DefaultPawn을 Spawn하고, Possess 합니다.
		 */
		


		
#endif

		
#ifdef CS_NM_DEDICATED
		/* 멀티플레이어 서버 이면,
		 * 0. PlayerStartActor를 검색하고, 없으면 접속을 거부. Return.
		 * 1. 새로운 PlayerController를 Spawn하고, Id를 지정합니다.
		 * 2. PlayerStartActor를 Destroy하고, PlayerStartActor로부터 새로운 DefaultPawn을 Spawn하고, Possess 합니다.
		 */
#endif





		
		

#if SERVER
		auto& startActors = GetAllActorByClass("PlayerStartActor");
		if(startActors.empty())
		{
			// 접속 거부
			return;
		}
		
		auto newPlayerController = SpawnActor<PlayerController>();
		// 키맵핑 here

		newPlayerController->SetNetworkId();
		DestroyActor(startActors[0]);
		auto newActor = SpawnActor<Actor>();
		newPlayerController->Possess(newActor);
		m_PlayerControllers.push_back(newPlayerController);
#endif

		
	}

	std::weak_ptr<Actor> Level::GetActorByName(const std::string& name)
	{
		auto it = FindActorItByDelegate([&name](const std::shared_ptr<Actor>& other)-> bool
		{
			return other->GetObjectName() == name;
		});

		if (it == m_Actors.end())
			return {};
		return *it;
	}

	std::weak_ptr<Actor> Level::GetActorByClass(const std::string& classType)
	{
		auto it = FindActorItByDelegate([&classType](const std::shared_ptr<Actor>& other)-> bool
		{
			return other->StaticType() == classType;
		});

		if (it == m_Actors.end())
			return {};
		return *it;
	}

	std::vector<std::weak_ptr<Actor>> Level::GetAllActorByClass(const std::string& classType)
	{
		std::vector<std::weak_ptr<Actor>> result;
		for (auto& actor : m_Actors)
		{
			if (actor->StaticType() != classType)
			{
				continue;
			}
			result.emplace_back(actor);
		}

		return result;
	}

	std::vector<std::shared_ptr<Actor>>::iterator Level::FindActorItByDelegate(
		const std::function<bool(const std::shared_ptr<Actor>&)>& delegate)
	{
		return std::find_if(m_Actors.begin(), m_Actors.end(), delegate);
	}

	bool Level::OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		auto playerController = Cast<PlayerController>(GetActorByClass("PlayerController"));
		if (playerController)
		{
			return playerController->OnInputEvent(hWnd, uMsg, wParam, lParam);
		}
	}

}
