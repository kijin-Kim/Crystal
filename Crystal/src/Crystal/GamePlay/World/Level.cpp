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
			if(actor->GetIsDead())
			{
				actor->End();
			}
		}
		m_PhysicsSystem->Update(deltaTime);


		//using Type = int;
		//std::unordered_map<Type, int> m_CountMap;
		//std::unordered_map<Type, int> m_CountMap2;
		////================¼­¹ö
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
		LineActor* debugLineActor = SpawnActor<LineActor>();
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
		
		for(auto it = m_Actors.begin(); it!= m_Actors.end();)
		{
			if(!(*it)->GetIsDead())
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

	std::shared_ptr<Actor> Level::GetActorByName(const std::string& name)
	{
		auto it = FindActorItByDelegate([&name](const std::shared_ptr<Actor>& other)-> bool
		{
			return other->GetObjectName() == name;
		});

		if (it == m_Actors.end())
			return nullptr;
		return *it;
	}

	std::shared_ptr<Actor> Level::GetActorByClass(const std::string& classType)
	{
		auto it = FindActorItByDelegate([&classType](const std::shared_ptr<Actor>& other)-> bool
		{
			return other->StaticType() == classType;
		});

		if (it == m_Actors.end())
			return nullptr;
		return *it;
	}

	std::vector<std::shared_ptr<Actor>> Level::GetAllActorByClass(const std::string& classType)
	{
		std::vector<std::shared_ptr<Actor>> result;
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
		if(playerController)
		{
			return playerController->OnInputEvent(hWnd, uMsg, wParam, lParam);
		}
	}

}
