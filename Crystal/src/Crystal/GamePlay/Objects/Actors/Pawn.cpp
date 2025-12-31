#include "cspch.h"
#include "Pawn.h"

#include "Crystal/GamePlay/Controllers/Controller.h"


namespace Crystal {

	void Pawn::AddInputVector(const DirectX::XMFLOAT3& direction, float scale)
	{
		m_InputVector = Vector3::Add(m_InputVector, Vector3::Multiply(direction, scale));
	}

	void Pawn::OnPossessed(Weak<Controller> controller)
	{
		m_Controller = controller;
	}

	void Pawn::Destroy()
	{
		Actor::Destroy();

		auto controller = Cast<Controller>(m_Controller);
		if(controller)
		{
			controller->Destroy();
		}
		
		
	}


}


