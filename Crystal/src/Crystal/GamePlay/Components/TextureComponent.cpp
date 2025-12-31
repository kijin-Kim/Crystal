#include "cspch.h"
#include "TextureComponent.h"

#include "MeshComponents.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"


namespace Crystal {


	void TextureComponent::RegisterComponent()
	{
		PrimitiveComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto scene = level->GetScene();

		scene->Textures.push_back(Cast<TextureComponent>(shared_from_this()));
	}

	void TextureComponent::UpdateTransforms()
	{
		PrimitiveComponent::UpdateTransforms();


		const auto position = GetLocalPosition();

		float scaleX = m_Scale.x;
		float scaleY = m_Scale.y;
		float scaleZ = 1.0f;


		if(!m_Materials.empty() && m_Materials[0])
		{
			auto albedoTexture = m_Materials[0]->AlbedoTexture.lock();
			auto opacityTexture = m_Materials[0]->OpacityTexture.lock();
			if (albedoTexture)
			{
				scaleX *= (float)albedoTexture->GetWidth();
				scaleY *= (float)albedoTexture->GetHeight();
			}
			else if (opacityTexture)
			{
				scaleX *= (float)opacityTexture->GetWidth();
				scaleY *= (float)opacityTexture->GetHeight();
			}
		}

		const auto scale = Matrix4x4::Scale({scaleX, scaleY, scaleZ});
		const DirectX::XMFLOAT4X4 rotation = Matrix4x4::
			RotationQuaternion(m_RotationQuat);
		const auto translation = Matrix4x4::Translation(position);

		m_LocalTransform = Matrix4x4::Multiply(scale, rotation);
		m_LocalTransform = Matrix4x4::Multiply(m_LocalTransform, translation);
		UpdateWorldTransform();
	}

}


