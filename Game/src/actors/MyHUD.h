#pragma once

#include "SpaceCombatTypes.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"


namespace Crystal {
	class TextureComponent;
}

class MyHUD : public Crystal::Actor
{
public:
	MyHUD() = default;
	~MyHUD() override = default;

	void Initialize() override;

	void Update(float deltaTime) override;

	void OnPlayerHealthUpdated(float currentHealth, float maxHealth) const;

	void OnKrakenHealthUpdated(float currentHealth, float maxHealth) const;


	void OnPolluteGaugeUpdated(float currentPollute, float maxPolluteGauge) const;

	void OnItemAcquired(ItemType itemType) const;

	void OnItemUsed(ItemType itemType) const;

	void OnPlayerHitSucceed();

	STATIC_TYPE_IMPLE(MyHUD)

private:
	Crystal::Shared<Crystal::TextureComponent> m_PlayerFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_KrakenFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_PolluteFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_PowerPotionIcon = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HealPotionIcon = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_ShieldPotionIcon = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HitmarkerTextureComponent = nullptr;
	int m_HealthBarWidth = 0;
	int m_HealthBarHeight = 0;

	float m_BarPadding = 0.0f;
	float m_BarBackgroundWidth = 0.0f;
	float m_FillWidth = 0.0f;

	DirectX::XMFLOAT2 m_PlayerBarScale = { 1.0f , 1.0f };
	DirectX::XMFLOAT2 m_KrakenBarScale = { 1.0f , 1.0f };
	DirectX::XMFLOAT2 m_PolluteBarScale = { 1.0f , 1.0f };


};