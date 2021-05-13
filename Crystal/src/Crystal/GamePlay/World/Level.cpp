#include "cspch.h"
#include "Level.h"


#include "Crystal/GamePlay/Objects/Actors/LineActor.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"


#include "../../../../../Game/src/actors/TestPawn.h"
#include "Crystal/Resources/ResourceManager.h"

namespace Crystal {
	void Level::OnCreate()
	{
		Object::OnCreate();

		m_PhysicsSystem = CreateShared<PhysicsSystem>();
		m_PhysicsSystem->Initialize();
		m_PhysicsSystem->SetObjectName("LevelPhysicsSystem");
		m_PhysicsSystem->OnCreate();

#if defined(CS_NM_STANDALONE) || defined(CS_NM_CLIENT)
		m_RenderSystem = CreateShared<RenderSystem>();
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

#ifdef CS_NM_DEDICATED // 멀티 플레이어 서버 인풋을 넘겨받은걸로 적절한 playercontroller 한테 넘겨줘야한다.
		for (const auto& playerController : m_PlayerControllers)
		{
			uint8_t id = playerController->GetNetworkId();
			playerController->OnInputEvent(여기다가 데이터);
		}
#endif

		AddPendingSpawnedActors();
		for (const auto& actor : m_Actors)
		{
			actor->UpdateComponents(deltaTime);
			actor->Update(deltaTime);
			if (actor->GetIsDead())
			{
				m_bHasDeadActors = true;
				actor->End();
			}
		}
		m_PhysicsSystem->Update(deltaTime);
		RemovePendingActors();

		// TODO : 렌더링 준비과정

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
		debugLineActor->SetLifeTime(1.5f);
	}

	void Level::AddActor(const std::shared_ptr<Actor>& actor)
	{
		m_PendingSpawnedActors.push_back(actor);
	}

	void Level::AddPendingSpawnedActors()
	{
		if (m_PendingSpawnedActors.empty())
		{
			return;
		}

		m_Actors.insert(m_Actors.end(), m_PendingSpawnedActors.begin(), m_PendingSpawnedActors.end());
		m_PendingSpawnedActors.clear();
	}

	void Level::AddPlayerController(const std::shared_ptr<PlayerController>& playerController)
	{
		m_PlayerControllers.push_back(playerController);
	}

	void Level::RemoveActor(const std::shared_ptr<Actor>& actor)
	{
		actor->SetIsDead(true);
	}

	void Level::RemovePendingActors()
	{
		if (!m_bHasDeadActors)
			return;

		m_bHasDeadActors = false;

		for (auto it = m_Actors.begin(); it != m_Actors.end();)
		{
			if (!(*it)->GetIsDead())
			{
				++it;
			}
			else
			{
				it = m_Actors.erase(it);
			}
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
		debugLineActor->SetLifeTime(1.5f);
	}

	void Level::RegisterPhysicsWorldComponent(std::weak_ptr<Component> component)
	{
		m_PhysicsSystem->RegisterPhysicsWorldComponent(component);
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
		AddPendingSpawnedActors();
#ifdef CS_NM_CLIENT
		/* 멀티플레이어 클라이언트면,
		* 서버가 알아서 생성
		* return;
		*
		* 
		*
		*/
		return;
#endif

#ifdef CS_NM_STANDALONE
		/* 스탠드얼론이 면,
		* 0. PlayerStartActor를 검색하고, 없으면 접속을 거부. Return.
		* 1. PlayerStartActor를 Destroy하고, PlayerStartActor로부터 새로운 DefaultPawn을 Spawn하고, Possess 합니다.
		* 2. 새로운 PlayerController를 Spawn합니다.
		*/
		auto& playerStartActors = GetAllActorByClass("PlayerStartActor");
		if (playerStartActors.empty())
		{
			CS_FATAL(false, "더 이상 클라이언트를 받을 수 없습니다");
			return;
		}

		auto playerStartActor = playerStartActors[0].lock();

		auto newActor = SpawnActor<TestPawn>({"TestPawn"}).lock();
		newActor->SetPosition({0.0f, 0.0f, -2000.0f});

		auto newActorMesh = Crystal::Cast<Crystal::StaticMeshComponent>(
			newActor->GetComponentByClass("StaticMeshComponent"));

		auto& resourceManager = ResourceManager::Instance();
		
		newActorMesh->SetRenderable(resourceManager.GetRenderable<StaticMesh>("assets/models/SM_Frigate_BE2.fbx"));
		auto newActorMat = newActorMesh->GetMaterial(0);
		newActorMat->AlbedoTexture = resourceManager.GetTexture("assets/textures/T_Frigate_BE2/T_M_SM_Frigate_BE2_MI_Frigate_BE2_White_BaseColor.tga");
		newActorMat->MetallicTexture = resourceManager.GetTexture("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Metallic.tga");
		newActorMat->RoughnessTexture = resourceManager.GetTexture("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Roughness.tga");
		newActorMat->NormalTexture = resourceManager.GetTexture("assets/textures/T_Frigate_BE2/T_Frigate_BE2_Norm.tga");

		newActor->SetScale(playerStartActor->GetScale());
		newActor->SetRotation(playerStartActor->GetRotation());
		newActor->SetPosition(playerStartActor->GetPosition());

		playerStartActor->Destroy();

		auto playerController = SpawnActor<PlayerController>().lock();
		playerController->Possess(newActor);


		// 시리얼라이즈
		std::ostringstream oss;
		boost::archive::text_oarchive os(oss);
		os << *newActor;


		// 디시리얼라이즈
		std::istringstream iss(oss.str());
		boost::archive::text_iarchive is(iss);
		is >> *newActor;


#endif


#ifdef CS_NM_DEDICATED
		/* 멀티플레이어 서버 이면,
		* 0. PlayerStartActor를 검색하고, 없으면 접속을 거부. Return.
		* 1. 새로운 PlayerController를 Spawn하고, Id를 지정합니다.
		* 2. PlayerStartActor를 Destroy하고, PlayerStartActor로부터 새로운 DefaultPawn을 Spawn하고, Possess 합니다.
		*/

		auto& playerStartActors = GetAllActorByClass("PlayerStartActor");
		if (playerStartActors.empty())
		{
			CS_FATAL(false, "더 이상 클라이언트를 받을 수 없습니다");
			return;
		}

		auto& resourceManager = ResourceManager::Instance();

		auto playerStartActor = playerStartActors[0].lock();


		auto newActor = SpawnActor<TestPawn>({ "TestPawn" }).lock();

		auto newActorMesh = Crystal::Cast<Crystal::StaticMeshComponent>(
			newActor->GetComponentByClass("StaticMeshComponent"));

		newActorMesh->SetRenderable(resourceManager.GetRenderable("Frigate"));
		auto newActorMat = newActorMesh->GetMaterial(0);
		newActorMat->AlbedoTexture = resourceManager.GetTexture("Frigate_Albedo");
		newActorMat->MetallicTexture = resourceManager.GetTexture("Frigate_Metallic");
		newActorMat->RoughnessTexture = resourceManager.GetTexture("Frigate_Roughness");
		newActorMat->NormalTexture = resourceManager.GetTexture("Frigate_Normal");

		newActor->SetScale(playerStartActor->GetScale());
		newActor->SetRotation(playerStartActor->GetRotation());
		newActor->SetPosition(playerStartActor->GetPosition());

		playerStartActor->Destroy();

		auto playerController = SpawnActor<PlayerController>().lock();
		playerController->SetNetworkId();
		playerController->Possess(newActor);


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

	std::weak_ptr<PlayerController> Level::GetPlayerController(int index)
	{
		if (m_PlayerControllers.size() <= index)
		{
			CS_WARN("Index : %d에 해당하는 플레이어 컨트롤러가 존재하지 않습니다.", index);
			return {};
		}

		return m_PlayerControllers[index];
	}

	std::weak_ptr<PlayerController> Level::GetPlayerControllerByNetworkId(int id)
	{
		auto it = std::find_if(m_PlayerControllers.begin(), m_PlayerControllers.end(),
		                       [&id](const std::shared_ptr<PlayerController>& other)
		                       {
			                       return other->GetNetworkId() == id;
		                       });

		if (it == m_PlayerControllers.end())
		{
			return {};
		}

		return *it;
	}

	std::vector<std::shared_ptr<Actor>>::iterator Level::FindActorItByDelegate(
		const std::function<bool(const std::shared_ptr<Actor>&)>& delegate)
	{
		return std::find_if(m_Actors.begin(), m_Actors.end(), delegate);
	}

	bool Level::OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
#ifdef CS_NM_STANDALONE
		
			
		bool bHandled = false;
		for (const auto& playerController : m_PlayerControllers)
		{
			bHandled |= playerController->OnInputEvent(hWnd, uMsg, wParam, lParam);
		}


		
		return bHandled;
#endif

#ifdef CS_NM_CLIENT // 멀티 플레이어 클라이언트


#endif
	}
}
