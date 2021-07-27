#pragma once
#include <vector>
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"
#include "Crystal/GamePlay/Components/InputComponent.h"

namespace Crystal {
	class Controller : public Actor
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<Actor>(*this);
		}

	public:
		Controller() = default;
		~Controller() override = default;

		virtual void Possess(std::shared_ptr<Pawn> pawn)
		{
			m_PossessedPawn = pawn;
			pawn->OnPossessed(Cast<Controller>(weak_from_this()));
		}

		virtual bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }


		Weak<Pawn> GetPossessedPawn() const { return m_PossessedPawn; }

		STATIC_TYPE_IMPLE(Controller)


	protected:
		Weak<Pawn> m_PossessedPawn;
	};
}
