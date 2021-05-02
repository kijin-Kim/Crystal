#pragma once
#include "Actor.h"
#include "ActorClassOf.h"


namespace Crystal {
	
	class GameMode : public Actor
	{
	public:
		ActorClassBase* GetDefaultActorClass() const;

		STATIC_TYPE_IMPLE(GameMode);
	
	public :
		std::unique_ptr<ActorClassBase> m_DefaultActorClass;
	};
}

