#include "cspch.h"
#include "World.h"
#include "Level.h"

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
		level->SetObjectOwner(weak_from_this(), Level::LevelOwnerType::Owner_World);

		m_Levels.push_back(std::move(level));

		return m_Levels.back().get();
	}

	Level* World::GetLevelByName(const std::string& name)
	{
		auto it = std::find_if(m_Levels.begin(), m_Levels.end(), [name](const std::shared_ptr<Level>& level)->bool
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

	void World::Update(const float deltaTime)
	{
		Updatable::Update(deltaTime);

		for (const auto& level : m_Levels)
			level->Update(deltaTime);
	}

}

