#include "cspch.h"
#include "World.h"

#include "Actors/Actor.h"
#include "Level.h"

namespace Crystal {

	World::World()
	{
	}

	World::~World()
	{

	}

	void World::Update(float DeltaTime)
	{
		for (Level* level : m_Levels)
			level->Update(DeltaTime);
	}

	void World::AddLevel(Level* level)
	{
		level->SetOwningWorld(this);
		m_Levels.push_back(level);
	}

	void World::RemoveLevel(Level* level)
	{
		// TODO : Think About OwnerShip and Life Time
		// Level LifeTime is not bound to World
		auto it = std::find(m_Levels.begin(), m_Levels.end(), level);
		if (it != m_Levels.end())
		{
			m_Levels.erase(it);
			level->SetOwningWorld(nullptr);
		}
		else
			CS_ASSERT(it != m_Levels.end(), "�����Ϸ��� ������ ã�� �� �����ϴ�.");
	}


	void World::SpawnActor(Actor* actor, Level* level)
	{
		actor->SetOwningLevel(level);
		auto it = std::find(m_Levels.begin(), m_Levels.end(), level);
		if (it != m_Levels.end())
		{
			level->SpawnActor(actor);
		}
		else
			CS_ASSERT(false, "������ ������ ���忡 ���ԵǾ����� �ʽ��ϴ�.");
	}

	void World::DestroyActor(Actor* actor)
	{
		Level* owningLevel = actor->GetOwningLevel();
		if (owningLevel)
		{
			owningLevel->DestroyActor(actor);
		}
		else
			CS_ASSERT(false, "�����Ϸ��� ���Ͱ� ����(����)�� �������� �ʽ��ϴ�.");
	}

}