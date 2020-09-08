#include "cspch.h"
#include "Level.h"

#include "Actors/Actor.h"
#include "Crystal/Renderer/Renderer.h"

namespace Crystal {

	void Level::Update(float DeltaTime)
	{
		for(Actor* actor : m_Actors)
			actor->Update(DeltaTime);
	}

	void Level::SpawnActor(Actor* actor)
	{
		actor->SetOwningLevel(this);
		m_Actors.push_back(actor);
		
		
	}

	void Level::DestroyActor(Actor* actor)
	{
		// Level Will Destroy Actor Itself

		auto it = std::find(m_Actors.begin(), m_Actors.end(), actor);
		if (it != m_Actors.end())
		{
			delete actor;
			actor = nullptr;
			m_Actors.erase(it);
		}
		else
			CS_ASSERT(false, "삭제하려는 액터가 월드(레벨)에 존재하지 않습니다.");

	}

}