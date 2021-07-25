#include "cspch.h"
#include "MovementComponent.h"
#include "Crystal/Gameplay/Objects/Actors/Pawn.h"

BOOST_CLASS_EXPORT(Crystal::MovementComponent)

namespace Crystal {


	Shared<Pawn> MovementComponent::GetTargetComponentOwner() const
	{
		return Cast<Pawn>(m_TargetComponent->GetOuter());
	}
}
