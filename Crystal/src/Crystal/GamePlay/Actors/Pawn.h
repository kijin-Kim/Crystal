#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/MeshComponent.h"
#include "Crystal/GamePlay/Components/InputComponent.h"

namespace Crystal {

	class Pawn : public Actor
	{
	public:
		Pawn()
		{
			////// TEMPORARY ////
			m_MeshComponent = new MeshComponent();
			m_MeshComponent->SetMesh(std::make_shared<Mesh>("assets/models/Megaphone_01.fbx"));
			//m_MeshComponent->SetMesh(std::make_shared<Mesh>("assets/models/SK_Mannequin.FBX"));
			m_MainComponent = m_MeshComponent;
			//m_MeshComponent->AttachToComponent(m_MainComponent);
		}

		virtual ~Pawn()
		{
			delete m_MeshComponent;
			delete m_InputComponent;
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

		void MoveForward(float value)
		{
			__debugbreak();
		}



	private:
		//////////////////Temporary//////////////
		// Need to be memeber of some specialized child class e.g. MeshActor
		MeshComponent* m_MeshComponent = nullptr;
		InputComponent* m_InputComponent = nullptr;
	};
}
