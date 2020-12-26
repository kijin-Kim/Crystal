#pragma once
#include "Component.h"
#include "Crystal/Core/GlobalTypes.h"


namespace Crystal {

	class Pawn;

	class InputComponent : public Component
	{
	public:
		InputComponent() = default;
		virtual ~InputComponent() = default;

		virtual void Update(float deltaTime) override
		{

		}
		
		void BindAxis(const std::string& axisName, Pawn* owner, const std::function<void(Pawn*, float value)>& function)
		{
		
		}
		void BindAction(const std::string& actionName, InputEventType eventType, Pawn* owner, const std::function<void(void)>& function)
		{
			//PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
		}

	private:
		std::map<int, InputEventType> m_InputMap;
	};

}