#include "cspch.h"
#include "World.h"
#include "Level.h"
#include "Crystal/GamePlay/Controllers/PlayerController.h"

namespace Crystal {


	void World::OnCreate()
	{
		Updatable::OnCreate();
	}

	Level* World::CreateNewLevel(const std::string& name /*= ""*/)
	{
		auto level = std::make_shared<Level>();
		level->OnCreate();
		if (!name.empty())
			level->SetObjectName(name);
		level->SetOwner(weak_from_this());

		m_Levels.push_back(std::move(level));

		return m_Levels.back().get();
	}

	Level* World::GetLevelByName(const std::string& name)
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

		return (*it).get();
	}



	Level* World::GetLevelByIndex(int index)
	{
		if (index >= m_Levels.size() || !m_Levels[index])
			return nullptr;

		return m_Levels[index].get();
	}

	Level* World::GetCurrentLevel()
	{
		return m_CurrentLevel;
	}

	void World::SetCurrentLevelByName(const std::string& name)
	{
		m_CurrentLevel = GetLevelByName(name);
	}

	void World::SetCurrentLevelByIndex(int index)
	{
		m_CurrentLevel = GetLevelByIndex(index);
	}



	void World::Update(const float deltaTime)
	{
		Updatable::Update(deltaTime);

		if (m_CurrentLevel)
		{
			m_CurrentLevel->Update(deltaTime);
		}
	}

}

