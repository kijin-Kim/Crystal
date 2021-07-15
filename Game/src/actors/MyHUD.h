#pragma once
#include "Crystal/GamePlay/Components/TextureComponent.h"
#include "Crystal/GamePlay/Objects/Actors/Actor.h"

class MyHUD : public Crystal::Actor
{
	SERIALIZE_PROPERTIES
	{
		boost::serialization::base_object<Crystal::Actor>(*this);
	}

public:
	MyHUD() = default;
	~MyHUD() override = default;

	void Initialize() override
	{
		auto crossHairMat = std::make_unique<Crystal::Material>();
		crossHairMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/RedCrosshair.tga");
		
		
		auto crossHairTexture = CreateComponent<Crystal::TextureComponent>("CrosshairTextureComponent");
		crossHairTexture->AddMaterial(std::move(crossHairMat));
		


		auto hpBarBgMat = std::make_unique<Crystal::Material>();
		hpBarBgMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hpBarBg.png");

		auto hpBarBg = CreateComponent<Crystal::TextureComponent>("HpBackgroundTextureComponent");
		hpBarBg->AddMaterial(std::move(hpBarBgMat));
		hpBarBg->SetScale(0.35f);
		hpBarBg->SetWorldPosition({ -1920.0f / 2.0f + 250.0f , 480.0f, 1.0f });


		auto hpBarFillMat = std::make_unique<Crystal::Material>();
		hpBarFillMat->AlbedoTexture = Crystal::ResourceManager::Instance().GetTexture("assets/textures/hpBarFill.png");

		auto hpBarFill = CreateComponent<Crystal::TextureComponent>("HpFillTextureComponent");
		hpBarFill->AddMaterial(std::move(hpBarFillMat));
		hpBarFill->SetScale(0.35f);
		hpBarFill->SetWorldPosition({ -1920.0f / 2.0f + 250.0f , 480.0f, 0.0f });
		

		
	}


	STATIC_TYPE_IMPLE(MyHUD)

private:
	

};
