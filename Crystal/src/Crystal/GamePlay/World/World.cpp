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

	Shared<Level> World::GetCurrentLevel() const
	{
		if(!m_LevelStack.empty())
		{
			return m_LevelStack.top();
		}
		return nullptr;
		
	}

	void World::PushLevel(const std::string& name)
	{
		auto it = std::find_if(m_Levels.begin(), m_Levels.end(), [&name](const std::shared_ptr<Level>& level)->bool
			{
				return level->GetObjectName() == name;
			}
		);
		if(it == m_Levels.end())
		{
			CS_WARN("%s 이름의 Level을 찾을 수 없습니다.", name.c_str());
		}

		Shared<Level> lastLevel = nullptr;
		if(!m_LevelStack.empty())
		{
			m_LevelStack.top()->OnLevelClosed(*it);
			lastLevel = m_LevelStack.top();
		}

		m_LevelStack.push(*it);
		m_LevelStack.top()->OnLevelOpened(lastLevel);
		m_RenderSystem->Begin();
	}

	void World::PopLevel()
	{
		if (!m_LevelStack.empty())
		{
			auto topLevel = m_LevelStack.top();
			m_LevelStack.pop();

			if (!m_LevelStack.empty())
			{
				topLevel->OnLevelClosed(m_LevelStack.top());
				m_LevelStack.top()->OnLevelOpened(topLevel);
			}
			else
			{
				topLevel->OnLevelClosed(nullptr);

			}
		}
	}


	bool World::OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if(!m_LevelStack.empty())
		{
			if(uMsg == WM_KEYDOWN && wParam == VK_F11)
			{
				GetRenderSystem()->ToggleDisplayMode();
				return true;
			}

			return m_LevelStack.top()->OnInputEvent(hWnd, uMsg, wParam, lParam);
		}
		
	}

	void World::Update(const float deltaTime)
	{
		Object::Update(deltaTime);

		m_LevelStack.top()->Update(deltaTime);
	}

}

