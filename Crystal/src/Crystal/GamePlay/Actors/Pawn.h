#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/MeshComponent.h"
#include "Crystal/GamePlay/Components/InputComponent.h"
#include "Crystal/Resources/ShaderManager.h"

namespace Crystal {
	class Pawn : public Actor
	{
	public:
		Pawn()
		{
		}

		virtual ~Pawn()
		{
		}

		virtual void Start() override
		{
			Actor::Start();
		}
		virtual void Update(float deltaTime) override
		{
			Actor::Update(deltaTime); // Updating All Component
		}

		virtual void SetupInputComponent(InputComponent* inputComponent)
		{
		}


	protected:
		std::shared_ptr<MeshComponent> m_MeshComponent = nullptr;
	};
}
