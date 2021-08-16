#include "cspch.h"
#include "World.h"
#include "Level.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

namespace Crystal {


	void World::OnCreate()
	{
		Object::OnCreate();

		m_RenderSystem = CreateShared<RenderSystem>();
		m_RenderSystem->Initialize();
		m_RenderSystem->SetOuter(weak_from_this());
		m_RenderSystem->SetObjectName("LevelRenderSystem");
		m_RenderSystem->OnCreate();

		m_PhysicsSystem = CreateShared<PhysicsSystem>();
		m_PhysicsSystem->Initialize();
		m_PhysicsSystem->SetOuter(weak_from_this());
		m_PhysicsSystem->SetObjectName("LevelPhysicsSystem");
		m_PhysicsSystem->OnCreate();
	}

	void World::DestroyActor(const std::shared_ptr<Actor>& actor)
	{
		auto level = Cast<Level>(actor->GetOuter());
		CS_FATAL(level, "Outer가 존재하지 않습니다");
		level->RemoveActor(actor);
	}

	

	const Shared<Level>& World::GetLevelByName(const std::string& name)
	{
		auto it = std::find_if(m_Levels.begin(), m_Levels.end(), [&name](const std::shared_ptr<Level>& level)->bool
			{
				return level->GetObjectName() == name;
			}
		);

		if (it == m_Levels.end())
		{
			CS_WARN("%s 이름의 Level을 찾을 수 없습니다.", name.c_str());
			return nullptr;
		}

		return (*it);
	}



	const Shared<Level>& World::GetLevelByIndex(int index)
	{
		if (index >= m_Levels.size() || !m_Levels[index])
			return nullptr;

		return m_Levels[index];
	}

	const Shared<Level>& World::GetCurrentLevel() const
	{
		return m_CurrentLevel;
	}

	void World::OpenLevel(const std::string& name)
	{
		m_OpenLevelCommands.push_back({ name });
	}

	void World::CloseLevel(const std::string& name)
	{
		m_CurrentLevel->OnLevelClosed();
	}


	bool World::OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(m_CurrentLevel)
		{
			if(uMsg == WM_KEYDOWN && wParam == VK_F11)
			{
				GetRenderSystem()->ToggleDisplayMode();
				return true;
			}

			return m_CurrentLevel->OnInputEvent(hWnd, uMsg, wParam, lParam);
		}
		
	}

	void World::Update(const float deltaTime)
	{
		Object::Update(deltaTime);

		if (m_CurrentLevel)
		{
			m_CurrentLevel->Update(deltaTime);
		}

		if(!m_OpenLevelCommands.empty())
		{
			for (auto& command : m_OpenLevelCommands)
			{
				if (m_CurrentLevel)
				{
					CloseLevel(m_CurrentLevel->GetObjectName());
				}
				m_CurrentLevel = GetLevelByName(command.LevelName);
				m_CurrentLevel->OnLevelOpened();
				m_RenderSystem->Begin();
			}

			m_OpenLevelCommands.clear();
		}

		


	}

}

