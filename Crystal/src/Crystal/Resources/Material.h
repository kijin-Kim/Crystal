#pragma once
#include <memory>
#include "Crystal/Resources/Texture.h"
#include "Crystal/Resources/Shader.h"
#include "Crystal/GamePlay/Objects/Object.h"

namespace Crystal {

	enum class EShadingModel
	{
		ShadingModel_Undefined,
		ShadingModel_Unlit,
		ShadingModel_DefaultLit
	};
	

	struct NewMaterial : public Object
	{
		std::weak_ptr<Texture> AlbedoTexture;
		DirectX::XMFLOAT3 AlbedoColor = Vector3::Zero;

		std::weak_ptr<Texture> MetallicTexture;
		float MetallicConstant = 0.0f;

		std::weak_ptr<Texture> RoughnessTexture;
		float RoughnessConstant = 0.0f;

		std::weak_ptr<Texture> NormalTexture;

		std::weak_ptr<Texture> EmissiveTexture;
		DirectX::XMFLOAT3 EmissiveColor = Vector3::Zero;

		EShadingModel ShadingModel = EShadingModel::ShadingModel_Undefined;

		bool UsingSameTextures(NewMaterial* material);

		STATIC_TYPE_IMPLE(NewMaterial)
	};
}