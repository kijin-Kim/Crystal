#pragma once
#include "Actor.h"
#include "Crystal/GamePlay/Components/MeshComponents.h"
#include "Crystal/GamePlay/Components/InputComponent.h"

namespace Crystal {
	class Controller;
	class Pawn : public Actor
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<Actor>(*this);
		}
		
	public:
		Pawn() = default;
		~Pawn() override = default;

		virtual void SetupInputComponent(InputComponent* inputComponent) {}

		void AddInputVector(const DirectX::XMFLOAT3& direction, float scale);

		virtual void OnPossessed(Weak<Controller> controller);


		const DirectX::XMFLOAT3& GetInputVector() const { return m_InputVector; }
		void ClearInputVector() { m_InputVector = Vector3::Zero; }



		STATIC_TYPE_IMPLE(Pawn)
	protected:
		DirectX::XMFLOAT3 m_InputVector = Vector3::Zero;
		Weak<Controller> m_Controller;
	};

}
