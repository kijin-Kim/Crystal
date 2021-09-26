#pragma once

#include "SpaceCombatTypes.h"
#include "Crystal/GamePlay/Components/TextRenderingComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"


namespace Crystal {
	class TextureComponent;
	class TextRenderingComponent;
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
	void OnCruiserHealthUpdated(float currentHealth, float maxHealth) const;
	void OnSpaceStationHealthUpdated(float currentHealth, float maxHealth) const;


	void OnPolluteGaugeUpdated(float currentPollute, float maxPolluteGauge) const;


	void OnPlayerHitSucceed();

	void OnGoldUpdated(uint32_t goldAmount);

	void OnActorHovered(Crystal::Weak<Actor> hovered, float distFromPlayer);
	void OnActorNotHovered();

	void SetQuestMainText(const std::wstring& text) { m_QuestMainText->SetText(text); }

	void ShowBossHud();
	void HideBossHud();

	void ShowCruiserHud();
	void HideCruiserHud();

	void ShowSpaceStationHud();
	void HideSpaceStationHud();


	void OnCruiserMoving();
	void OnCruiserStopped();

	

	STATIC_TYPE_IMPLE(MyHUD)
	
private:
	Crystal::Shared<Crystal::TextureComponent> m_HoveredDetailBackgroundTexture = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HoveredTexture = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_PlayerFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_KrakenFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_KrakenBackgroundComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_CruiserFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_CruiserBackgroundComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_SpaceStationFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_SpaceStationBackgroundComponent = nullptr;


	Crystal::Shared<Crystal::TextureComponent> m_PolluteFillComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_PolluteBackgroundComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_PowerPotionIcon = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HealPotionIcon = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_ShieldPotionIcon = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_HitmarkerTextureComponent = nullptr;
	Crystal::Shared<Crystal::TextRenderingComponent> m_PolluteBarText = nullptr;
	Crystal::Shared<Crystal::TextRenderingComponent> m_KrakenBarText = nullptr;
	Crystal::Shared<Crystal::TextRenderingComponent> m_CruiserBarText = nullptr;
	Crystal::Shared<Crystal::TextRenderingComponent> m_SpaceStationBarText = nullptr;
	Crystal::Shared<Crystal::TextRenderingComponent> m_GoldTextComponent = nullptr;
	Crystal::Shared<Crystal::TextRenderingComponent> m_HoveredDetailNameText = nullptr;
	Crystal::Shared<Crystal::TextRenderingComponent> m_HoveredDetailDistanceText = nullptr;


	Crystal::Shared<Crystal::TextRenderingComponent> m_PowerPotionCount = nullptr;
	Crystal::Shared<Crystal::TextRenderingComponent> m_HealPotionCount = nullptr;
	Crystal::Shared<Crystal::TextRenderingComponent> m_ShieldPotionCount = nullptr;
	Crystal::Shared<Crystal::TextRenderingComponent> m_UnderAttackText = nullptr;


	Crystal::Shared<Crystal::TextRenderingComponent> m_QuestMainText = nullptr;
	std::vector<Crystal::Shared<Crystal::TextureComponent>> m_QuestCompletedMarkerOutlines;
	std::vector<Crystal::Shared<Crystal::TextureComponent>> m_QuestCompletedMarkers;
	std::vector<Crystal::Shared<Crystal::TextRenderingComponent>> m_QuestDisplayTexts;
	Crystal::Shared<Crystal::TextRenderingComponent> m_CruiserStoppedText;


	int m_HealthBarWidth = 0;
	int m_HealthBarHeight = 0;

	float m_BarPadding = 0.0f;
	float m_BarBackgroundWidth = 0.0f;
	float m_FillWidth = 0.0f;

	float m_HoveredTextureMaxScale = 0.0f;
	float m_HoveredTextureWidth = 0.0f;
	float m_HoveredDetailedBackgroundTextureWidth = 0.0f;
	float m_HoveredDetailedBackgroundTextureHeight = 0.0f;


	DirectX::XMFLOAT2 m_PlayerBarScale = {1.0f, 1.0f};
	DirectX::XMFLOAT2 m_KrakenBarScale = {1.0f, 1.0f};
	DirectX::XMFLOAT2 m_CruiserBarScale = {1.0f, 1.0f};
	DirectX::XMFLOAT2 m_SpaceStationBarScale = {1.0f, 1.0f};
	DirectX::XMFLOAT2 m_PolluteBarScale = {1.0f, 1.0f};
};
