#include "cspch.h"
#include "Level.h"


#include "Crystal/Renderer/Scene.h"

#include "Crystal/GamePlay/Objects/Actors/LineActor.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"


#include "../../../../../Game/src/actors/MyPlayerPawn.h"
#include "Crystal/Resources/ResourceManager.h"

#include "../../../../../Game/src/actors/MyHUD.h"
#include "../../../../../Game/src/actors/Quest.h"
#include "Crystal/GamePlay/Objects/Actors/PlayerStartActor.h"

namespace Crystal {


	void Level::OnLevelOpened(Shared<Level> lastLevel)
	{
		m_Scene->Reset();
		for(auto& actor : m_Actors)
		{
			actor->RegisterComponents();
		}
	}

	void Level::OnLevelClosed(Shared<Level> nextLevel)
	{
		m_Quests.clear();
		m_Scene->Reset();
	}

	void Level::MoveActorToLevel(Shared<Actor> actor, Shared<Level> level)
	{
		level->OnActorMoved(actor);
		m_Actors.erase(std::find(m_Actors.begin(), m_Actors.end(), actor));
	}

	void Level::OnActorMoved(Shared<Actor> actor)
	{
		AddActor(actor);
		actor->SetOuter(weak_from_this());
	}


	void Level::ClearActors()
	{
		m_Scene->Reset();
		for (const auto& actor : m_Actors)
		{
			if (actor->StaticType() != "PlayerStartActor")
			{
				actor->Destroy();
			}
		}

		m_bHasDeadActors = true;
		RemovePendingActors();

		m_PlayerControllers.clear();
		m_Player = nullptr;
		m_HUD = nullptr;
	}


	void Level::Initialize()
	{
		Object::Initialize();
		m_Scene = CreateShared<Scene>();
	}

	void Level::Begin()
	{
		Object::Begin();

		auto world = Crystal::Cast<Crystal::World>(GetWorld());
		world->GetRenderSystem()->LoadCubemapTextures(m_Scene);
	}

	void Level::Update(const float deltaTime)
	{
		Object::Update(deltaTime);

		m_DeltaTime = deltaTime;

		m_bIsUpdating = true;

		AddPendingSpawnedActors();

		for (const auto& actor : m_Actors)
		{
			actor->Update(deltaTime);
			actor->UpdateComponents(deltaTime);
			if (actor->GetIsDead())
			{
				m_bHasDeadActors = true;
				actor->End();
			}
		}

		for(const auto& quest : m_Quests)
		{
			quest->Update(deltaTime);
		}

		if (!m_Quests.empty())
		{
			bool m_bAllCompleted = true;
			for (int i = 0; i < m_Quests.size(); i++)
			{
				m_bAllCompleted &= m_Quests[i]->GetIsCompleted();
			}
			if (m_bAllCompleted)
			{
				auto world = GetWorld().lock();
				world->PopLevel();
				world->PushLevel("LevelClearedLevel");
			}
		}

		auto world = Cast<World>(GetWorld());
		auto physicsSystem = world->GetPhysicsSystem();
		physicsSystem->Update(deltaTime);
		RemovePendingActors();

		// TODO : 렌더링 준비과정


		auto renderSystem = world->GetRenderSystem();
		if (renderSystem)
		{
			renderSystem->Update(deltaTime);
		}

		m_bIsUpdating = false;
	}

	void Level::DrawDebugLine(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float maxDistance,
	                          const DirectX::XMFLOAT3& color /*= { 0.0f, 1.0f, 0.0f }*/)
	{
		auto debugLineActor = SpawnActor<LineActor>({}).lock();
		auto lineComponent = debugLineActor->GetLineComponent();
		lineComponent->SetOrigin(origin);
		lineComponent->SetDirection(direction);
		lineComponent->SetMaxDistance(maxDistance);
		debugLineActor->SetLifeTime(1.5f);
	}

	void Level::AddActor(const std::shared_ptr<Actor>& actor)
	{
		if (m_bIsUpdating)
		{
			m_PendingSpawnedActors.push_back(actor);
			return;
		}

		m_Actors.push_back(actor);
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
				for (auto& quest : m_Quests)
				{
					quest->OnActorDestroyed((*it)->StaticType());
				}
				it = m_Actors.erase(it);
			}
		}
	}

	void Level::DrawDebugLine(const DirectX::XMFLOAT3& startPoint, const DirectX::XMFLOAT3& endPoint,
	                          const DirectX::XMFLOAT3& color /*= { 0.0f, 1.0f, 0.0f }*/)
	{
		auto debugLineActor = SpawnActor<LineActor>({}).lock();
		auto lineComponent = debugLineActor->GetLineComponent();

		const auto endSubStart = Vector3::Subtract(endPoint, startPoint);

		const auto direction = Vector3::Normalize(endSubStart);
		const auto maxDistance = Vector3::Length(endSubStart);

		lineComponent->SetOrigin(startPoint);
		lineComponent->SetDirection(direction);
		lineComponent->SetMaxDistance(maxDistance);
		debugLineActor->SetLifeTime(1.5f);
	}



	const Shared<Scene>& Level::GetScene() const
	{
		return m_Scene;
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


	void Level::CreateQuest(const std::string& displayText, const DirectX::XMFLOAT3& targetLocation, Weak<Actor> targetActor, const QuestReward& reward)
	{
		auto newQuest = CreateObject<Quest>(displayText, weak_from_this());
		newQuest->SetQuestType(Quest::EQuestType::QT_Location);
		newQuest->SetQuestText(displayText);
		newQuest->SetQuestTargetActor(targetActor);
		newQuest->SetQuestTargetLocation(targetLocation);
		newQuest->SetQuestReward(reward);

		m_Quests.push_back(newQuest);
	}

	void Level::CreateQuest(const std::string& displayText, const std::initializer_list<std::string>& types, uint32_t count, const QuestReward& reward)
	{
		auto newQuest = CreateObject<Quest>(displayText, weak_from_this());
		newQuest->SetQuestType(Quest::EQuestType::QT_Destroy);
		newQuest->SetQuestText(displayText);

		for(auto type : types)
		{
			newQuest->SetQuestTargetType(type);
		}
		newQuest->SetQuestTargetTypeCount(count);
		newQuest->SetQuestReward(reward);

		m_Quests.push_back(newQuest);
	}

	const std::vector<Shared<Quest>>& Level::GetQuests() const
	{
		return m_Quests;
	}

	std::vector<std::shared_ptr<Actor>>::iterator Level::FindActorItByDelegate(
		const std::function<bool(const std::shared_ptr<Actor>&)>& delegate)
	{
		return std::find_if(m_Actors.begin(), m_Actors.end(), delegate);
	}

	bool Level::OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		bool bHandled = false;

		if(!m_PlayerControllers.empty())
		{
			bHandled |= m_PlayerControllers[0]->OnInputEvent(hWnd, uMsg, wParam, lParam);
			if(bHandled)
			{
				return true;
			}
		}
		return bHandled;
	}

	bool Level::LineTraceSingle(HitResult& outHitResult, const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction, float& dist,
	                            const CollisionParams& collisionParams)
	{
		return Crystal::Cast<World>(GetWorld())->GetPhysicsSystem()->LineTraceSingle(outHitResult, origin, direction, dist, collisionParams);
	}

	const WorldConfig& Level::GetWorldConfig() const
	{
		return Cast<World>(GetOuter())->GetWorldConfig();
	}

	Weak<World> Level::GetWorld() const
	{
		return Cast<World>(GetOuter());
	}
}
