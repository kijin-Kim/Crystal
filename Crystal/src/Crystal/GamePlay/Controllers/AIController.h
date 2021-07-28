#pragma once
#include "Controller.h"

namespace Crystal {
	class BlackboardComponent;
}

namespace Crystal {
	class BehaviorTree;
	class BehaviorTreeComponent;

	class AIController : public Controller
	{
	public:
		AIController() = default;
		~AIController() override = default;

		void Initialize() override;

		void SetBehaviorTree(Shared<BehaviorTree> behaviorTree);

		Shared<BlackboardComponent> GetBlackboardComponent() const;
		

		void MoveToLocation(const DirectX::XMFLOAT3& location, float maxAccel, float acceptableRadius = 2.0f);

		void ProcessMoveToLocationRequest();


		void Update(float deltaTime) override;


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

		std::shared_ptr<BehaviorTreeComponent> m_BehaviorTreeComponent = nullptr;
		std::shared_ptr<BlackboardComponent> m_BlackboardComponent = nullptr;
		
	};

	
}
