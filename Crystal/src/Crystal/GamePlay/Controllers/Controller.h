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
		virtual ~Controller() = default;

		virtual void Possess(std::shared_ptr<Pawn> pawn)
		{
			m_PossessedPawn = pawn;
		}

		virtual bool OnInputEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }


		Weak<Pawn> GetPossessedPawn() const { return m_PossessedPawn; }

		STATIC_TYPE_IMPLE(Controller)


	private:
		Weak<Pawn> m_PossessedPawn;
	};
}
