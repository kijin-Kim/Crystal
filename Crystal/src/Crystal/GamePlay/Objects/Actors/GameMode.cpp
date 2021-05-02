#include "cspch.h"
#include "GameMode.h"

namespace Crystal {

	ActorClassBase* GameMode::GetDefaultActorClass() const
	{
		return m_DefaultActorClass.get();
	}
}
