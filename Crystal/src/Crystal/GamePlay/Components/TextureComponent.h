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


		STATIC_TYPE_IMPLE(TextureComponent)
	};

}
