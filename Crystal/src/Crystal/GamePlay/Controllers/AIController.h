#pragma once
#include "Controller.h"

namespace Crystal {

	class AIController : public Controller
	{
	public:
		AIController() = default;
		~AIController() override = default;

		void MoveToLocation(const DirectX::XMFLOAT3& location, float maxAccel, float acceptableRadius = 2.0f)
		{
			m_MoveToLocationRequest.TargetLocation = location;
			m_MoveToLocationRequest.bShouldMoveToTargetLocation = true;
			m_MoveToLocationRequest.MaxAcceleration = maxAccel;
			m_MoveToLocationRequest.AcceptableRadius = acceptableRadius;
		}

		void ProcessMoveToLocationRequest()
		{
			auto pawn = m_PossessedPawn.lock();
			if (!pawn)
			{
				return;
			}
				
			auto mainComponent = pawn->GetMainComponent().lock();
			if(!mainComponent)
			{
				return;
			}

			if(!mainComponent->HasFiniteMass())
			{
				return;
			}
			
			if (!m_MoveToLocationRequest.bShouldMoveToTargetLocation)
			{
				return;
			}

			
			auto distance = Vector3::Subtract(m_MoveToLocationRequest.TargetLocation, mainComponent->GetWorldPosition());
			float lengthSq = Vector3::LengthSquared(distance);
			float acceptableRadiusSq = m_MoveToLocationRequest.AcceptableRadius * m_MoveToLocationRequest.AcceptableRadius;

			if (lengthSq <= acceptableRadiusSq)
			{
				m_MoveToLocationRequest = {};
				return;
			}

			auto direction = Vector3::Normalize(distance);

			mainComponent->AddForce(Vector3::Multiply(direction, m_MoveToLocationRequest.MaxAcceleration));
		}


		void Update(float deltaTime) override
		{
			Controller::Update(deltaTime);

			ProcessMoveToLocationRequest();
			
			
		}

	
		STATIC_TYPE_IMPLE(AIController)

	private:
		struct MoveToLocationRequest
		{
			
			DirectX::XMFLOAT3 TargetLocation = Vector3::Zero;
			float MaxAcceleration = 0.0f;
			float AcceptableRadius = 0.0f;
			bool bShouldMoveToTargetLocation = false;
		};


		MoveToLocationRequest m_MoveToLocationRequest = {};
		
		
	};

	
}
