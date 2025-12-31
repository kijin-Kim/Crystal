#pragma once
#include "PrimitiveComponent.h"

namespace Crystal {

	class TextureComponent : public PrimitiveComponent
	{
	public:
		TextureComponent() = default;
		~TextureComponent() override = default;

		void RegisterComponent() override;

		void UpdateTransforms() override;


		void SetScaleX(float scale) { m_Scale.x = scale; }
		void SetScaleY(float scale) { m_Scale.y = scale; }


		STATIC_TYPE_IMPLE(TextureComponent)

	};

}
