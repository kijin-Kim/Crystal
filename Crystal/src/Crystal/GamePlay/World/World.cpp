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
		CS_FATAL(level, "Outer�� �������� �ʽ��ϴ�");
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
			CS_WARN("%s �̸��� Level�� ã�� �� �����ϴ�.", name.c_str());
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
		return m_LevelStack.top();
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
			CS_WARN("%s �̸��� Level�� ã�� �� �����ϴ�.", name.c_str());
		}
		
		m_LevelStack.push(*it);
		m_LevelStack.top()->OnLevelOpened();
		m_RenderSystem->Begin();
	}

	void World::PopLevel(const std::string& name)
	{
		m_LevelStack.top()->OnLevelClosed();
		m_LevelStack.pop();
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

