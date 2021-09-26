#pragma once
#include "TextureComponent.h"

namespace Crystal {

	struct AABB2D
	{
		DirectX::XMFLOAT2 Min;
		DirectX::XMFLOAT2 Max;
	};

	enum class EButtonState
	{
		BS_Pressed,
		BS_Released,
		BS_Hovered,
		BS_Unhovered
	};

	class ButtonComponent : public TextureComponent
	{
	public:
		ButtonComponent() = default;
		~ButtonComponent() override = default;

		void RegisterComponent() override;



		void BindOnButtonClickedEvent(std::function<void()> event);
		void BindOnButtonPressedEvent(std::function<void()> event);
		void BindOnButtonReleasedEvent(std::function<void()> event);
		void BindOnButtonHoveredEvent(std::function<void()> event);
		void BindOnButtonUnhoveredEvent(std::function<void()> event);


		void OnButtonClicked();
		void OnButtonPressed();
		void OnButtonReleased();
		void OnButtonHovered();
		void OnButtonUnhovered();


		void SetNormalMaterial(Shared<Material> material);
		void SetHoveredMaterial(Shared<Material> material);
		void SetPressedMaterial(Shared<Material> material);

		Shared<Material> GetNormalMaterial() const;
		Shared<Material> GetHoveredMaterial() const;
		Shared<Material> GetPressedMaterial() const;
		


		void Update(float deltaTime) override;


		AABB2D GetAABB() const;

		EButtonState GetButtonRenderState() const;

		bool GetIsHovered()const;


		STATIC_TYPE_IMPLE(ButtonComponent)

	private:
		std::function<void()> m_OnButtonClickedEvent;
		std::function<void()> m_OnButtonPressedEvent;
		std::function<void()> m_OnButtonReleasedEvent;
		std::function<void()> m_OnButtonHoveredEvent;
		std::function<void()> m_OnButtonUnhoveredEvent;

		EButtonState m_ButtonRenderState = EButtonState::BS_Unhovered;

		bool m_bIsHovered = false;


		AABB2D m_AABB;
	};

}
