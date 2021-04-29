#pragma once
#include "Actor.h"


namespace Crystal {
	class PlayerStartActor : public Actor
	{
	public:
		PlayerStartActor(const ObjectInitializer& objectInitializer) : Actor(objectInitializer)
		{
			//auto gameMode = GetWorld()->GetGameMode();
		}

		~PlayerStartActor() override = default;

		void Begin() override
		{
		}

		void End() override
		{
		}

		void Update(const float deltaTime) override
		{
		}


		STATIC_TYPE_IMPLE(PlayerStartActor)
	};
}
