#pragma once
#include "PrimitiveComponent.h"

namespace Crystal {

	class TextureComponent : public PrimitiveComponent
	{
		SERIALIZE_PROPERTIES
		{
			boost::serialization::base_object<PrimitiveComponent>(*this);
		}

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