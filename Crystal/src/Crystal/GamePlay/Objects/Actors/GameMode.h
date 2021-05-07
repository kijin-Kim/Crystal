#pragma once
#include "Actor.h"


namespace Crystal {

	class GameMode : public Actor
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<Actor>(*this);
		}
	};
}
