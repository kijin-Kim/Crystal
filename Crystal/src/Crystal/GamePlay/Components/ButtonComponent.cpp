#include "cspch.h"
#include "ButtonComponent.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"

namespace Crystal {


	void ButtonComponent::RegisterComponent()
	{
		PrimitiveComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto scene = level->GetScene();

		scene->Buttons.push_back(Cast<ButtonComponent>(shared_from_this()));
	}


	void ButtonComponent::BindOnButtonClickedEvent(std::function<void()> event)
	{
		m_OnButtonClickedEvent = event;
	}

	void ButtonComponent::BindOnButtonPressedEvent(std::function<void()> event)
	{
		m_OnButtonPressedEvent = event;
	}

	void ButtonComponent::BindOnButtonReleasedEvent(std::function<void()> event)
	{
		m_OnButtonReleasedEvent = event;
	}

	void ButtonComponent::BindOnButtonHoveredEvent(std::function<void()> event)
	{
		m_OnButtonHoveredEvent = event;
	}

	void ButtonComponent::BindOnButtonUnhoveredEvent(std::function<void()> event)
	{
		m_OnButtonUnhoveredEvent = event;
	}

	void ButtonComponent::OnButtonClicked()
	{
		if (m_OnButtonClickedEvent)
		{
			m_OnButtonClickedEvent();
		}
	}

	void ButtonComponent::OnButtonPressed()
	{
		m_ButtonRenderState = EButtonState::BS_Pressed;
		if (m_OnButtonPressedEvent)
		{
			m_OnButtonPressedEvent();
		}
	}

	void ButtonComponent::OnButtonReleased()
	{
		m_ButtonRenderState = EButtonState::BS_Released;
		if(m_bIsHovered)
		{
			m_ButtonRenderState = EButtonState::BS_Hovered;
		}
		
		if (m_OnButtonReleasedEvent)
		{
			m_OnButtonReleasedEvent();
		}
	}


	void ButtonComponent::OnButtonHovered()
	{
		if (m_bIsHovered)
		{
			return;
		}

		m_bIsHovered = true;
		m_ButtonRenderState = EButtonState::BS_Hovered;
		if (m_OnButtonHoveredEvent)
		{
			m_OnButtonHoveredEvent();
		}
	}

	void ButtonComponent::OnButtonUnhovered()
	{
		m_bIsHovered = false;
		m_ButtonRenderState = EButtonState::BS_Unhovered;
		if (m_OnButtonUnhoveredEvent)
		{
			m_OnButtonUnhoveredEvent();
		}
	}

	void ButtonComponent::SetNormalMaterial(Shared<Material> material)
	{
		if(!m_Materials.empty())
		{
			m_Materials[0] = material;
			return;
		}
		AddMaterial(material);
		
	}

	void ButtonComponent::SetHoveredMaterial(Shared<Material> material)
	{
		if (m_Materials.size() >= 2)
		{
			m_Materials[1] = material;
			return;
		}
		AddMaterial(material);
	}

	void ButtonComponent::SetPressedMaterial(Shared<Material> material)
	{
		if (m_Materials.size() >= 3)
		{
			m_Materials[2] = material;
			return;
		}
		AddMaterial(material);
	}

	Shared<Material> ButtonComponent::GetNormalMaterial() const
	{
		if (!m_Materials.empty())
		{
			return m_Materials[0];
		}
		return nullptr;
	}

	Shared<Material> ButtonComponent::GetHoveredMaterial() const
	{
		if (m_Materials.size() >= 2)
		{
			return m_Materials[1];
		}
		return nullptr;
	}

	Shared<Material> ButtonComponent::GetPressedMaterial() const
	{
		if (m_Materials.size() >= 3)
		{
			return m_Materials[2];
		}
		return nullptr;
	}

	void ButtonComponent::Update(float deltaTime)
	{
		TextureComponent::Update(deltaTime);

		DirectX::XMFLOAT3 position = GetLocalPosition();

		DirectX::XMFLOAT2 extent = {0.0f, 0.0f};

		if(!m_Materials.empty() && m_Materials[0])
		{
			auto albedoTexture = m_Materials[0]->AlbedoTexture.lock();
			auto opacityTexture = m_Materials[0]->OpacityTexture.lock();
			if (albedoTexture)
			{
				extent.x = albedoTexture->GetWidth() * m_Scale.x;
				extent.y = albedoTexture->GetHeight() * m_Scale.y;
			}
			else if (opacityTexture)
			{
				extent.x = opacityTexture->GetWidth() * m_Scale.x;
				extent.y = opacityTexture->GetHeight() * m_Scale.y;
			}
		}
		

		m_AABB.Min = Vector2::Subtract({position.x, position.y}, {extent.x, extent.y});
		m_AABB.Max = Vector2::Add({position.x, position.y}, {extent.x, extent.y});
	}

	AABB2D ButtonComponent::GetAABB() const
	{
		return m_AABB;
	}

	EButtonState ButtonComponent::GetButtonRenderState() const
	{
		return m_ButtonRenderState;
	}

	bool ButtonComponent::GetIsHovered() const
	{
		return m_bIsHovered;
	}


}


