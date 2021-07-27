#include "cspch.h"
#include "Pawn.h"

BOOST_CLASS_EXPORT(Crystal::Pawn)

namespace Crystal {

	void Pawn::AddInputVector(const DirectX::XMFLOAT3& direction, float scale)
	{
		m_InputVector = Vector3::Add(m_InputVector, Vector3::Multiply(direction, scale));
	}

	void Pawn::OnPossessed(Weak<Controller> controller)
	{
		m_Controller = controller;
	}


}
