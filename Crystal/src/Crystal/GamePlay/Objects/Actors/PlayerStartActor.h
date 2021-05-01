#pragma once
#include "Actor.h"


namespace Crystal {
	/*
	 * Placeholder of player actor
	 */
	
	class PlayerStartActor : public Actor
	{
	public:
		PlayerStartActor() = default;
		~PlayerStartActor() override = default;

		STATIC_TYPE_IMPLE(PlayerStartActor)
	
	};
}
