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
		ShadingModel_DefaultLit,
		ShadingModelCount
	};
	

	struct Material : public Object
	{
		Shared<Texture> AlbedoTexture = nullptr;
		DirectX::XMFLOAT3 AlbedoColor = Vector3::Zero;

		Shared<Texture> MetallicTexture = nullptr;
		float MetallicConstant = 0.0f;

		Shared<Texture> RoughnessTexture = nullptr;
		float RoughnessConstant = 0.0f;

		Shared<Texture> NormalTexture = nullptr;

		Shared<Texture> EmissiveTexture = nullptr;
		DirectX::XMFLOAT3 EmissiveColor = Vector3::Zero;

		EShadingModel ShadingModel = EShadingModel::ShadingModel_Undefined;

		bool UsingSameTextures(Material* material);

		STATIC_TYPE_IMPLE(NewMaterial)
	};
}