#pragma once
#include <vector>

#include "Crystal/GamePlay/Actors/Actor.h"
#include "Crystal/GamePlay/Components/MeshComponent.h"

namespace Crystal {


	class Level final
	{
	public:
		Level() = default;
		~Level() = default;

		void AddActor(Actor* actor) { m_Actors.push_back(actor); }
		//void RemoveActor() {}

	private:
		std::vector<Actor*> m_Actors;
	};

	class World final 
	{
	public:
		World()
		{
			m_Levels.push_back(new Level()); //Default Level
		}
		~World() = default;
	
		template<class T>
		T* SpawnActor(Level* level = nullptr)
		{
			// Need some validation like type checking... 
			
			T* newActor = new T();

			if (level == nullptr)
				m_Levels[0]->AddActor(newActor);

			Renderer::Instance().RegisterMesh(newActor->GetMeshComponent());
			

			return newActor;
		}

	private:
		std::vector<Level*> m_Levels;
	};


}
