#pragma once
#include "Controller.h"

namespace Crystal {

	class AIController : public Controller
	{
	public:
		AIController() = default;
		~AIController() override = default;

		void MoveToLocation(const DirectX::XMFLOAT3& location)
		{
			m_TargetLocation = location;
			m_bShouldMoveToTargetLocation = true;
		}

		void Update(float deltaTime) override
		{
			Controller::Update(deltaTime);

			if(m_bShouldMoveToTargetLocation)
			{
				
			}
			
		}

	
		STATIC_TYPE_IMPLE(AIController)

	private:
		DirectX::XMFLOAT3 m_TargetLocation = Vector3::Zero;
		bool m_bShouldMoveToTargetLocation = false;
		
	};

	
}
