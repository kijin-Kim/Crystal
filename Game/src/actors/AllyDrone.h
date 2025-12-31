#pragma once
#include "Crystal/GamePlay/Objects/Actors/Pawn.h"

namespace Crystal {
	class TextureComponent;
}

class AllyDrone : public Crystal::Pawn
{
public:
	AllyDrone() = default;
	~AllyDrone() override = default;

	void Initialize() override;

	void Update(float deltaTime) override;

	void OnFire();


	STATIC_TYPE_IMPLE(AllyDrone)

private:
	Crystal::Shared<Crystal::TransformComponent> m_FireSocketComponent = nullptr;
	Crystal::Shared<Crystal::TextureComponent> m_IndicatorTextureComponent = nullptr;
};


