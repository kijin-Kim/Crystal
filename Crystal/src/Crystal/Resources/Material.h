#pragma once
#include <memory>

#include "Crystal/Types.h"
#include "Crystal/Resources/Texture.h"
#include "Crystal/Resources/Shader.h"
#include "Crystal/GamePlay/Objects/Object.h"

namespace Crystal {



	struct Material : public Object
	{
		Weak<Texture> AlbedoTexture = {};
		DirectX::XMFLOAT3 AlbedoColor = Vector3::Zero;
		DirectX::XMFLOAT3 TintColor = {1.0f, 1.0f, 1.0f };

		Weak<Texture> MetallicTexture = {};
		float MetallicConstant = 0.0f;

		Weak<Texture> RoughnessTexture = {};
		float RoughnessConstant = 0.0f;

		Weak<Texture> NormalTexture = {};

		Weak<Texture> EmissiveTexture = {};
		DirectX::XMFLOAT3 EmissiveColor = Vector3::Zero;

		Weak<Texture> OpacityTexture = {};
		Weak<Texture> SceneColorMaskTexture = {};

		bool bUseAlbedoTextureAlpha = false;

		float Opacity = 1.0f;
		float OpacityMultiplier = 1.0f;

		bool bTwoSided = false;

		EShadingModel ShadingModel = EShadingModel::SM_Lit;
		EBlendMode BlendMode = EBlendMode::BM_Opaque;

		DirectX::XMFLOAT2 TexCoordMultiplier = { 1.0f, 1.0f };

		bool UsingSameTextures(Material* material);

		

		STATIC_TYPE_IMPLE(Material)
	};
}