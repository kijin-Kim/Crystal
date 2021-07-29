#include "cspch.h"
#include "TextureComponent.h"

#include "MeshComponents.h"
#include "Crystal/GamePlay/World/Level.h"
#include "Crystal/Renderer/Scene.h"

BOOST_CLASS_EXPORT(Crystal::TextureComponent)

namespace Crystal {


	void TextureComponent::RegisterComponent()
	{
		PrimitiveComponent::RegisterComponent();

		auto owner = Cast<Actor>(GetOuter());
		auto level = Cast<Level>(owner->GetOuter());
		auto& scene = level->GetScene();

#ifndef CS_NM_DEDICATED
		scene->Textures.push_back(Cast<TextureComponent>(shared_from_this()));
#endif

		
	}

	void TextureComponent::UpdateTransforms()
	{
		PrimitiveComponent::UpdateTransforms();


		const auto position = GetLocalPosition();

		float scaleX = m_Scale.x;
		float scaleY = m_Scale.y;
		float scaleZ = 1.0f;

		auto texture = m_Materials[0]->AlbedoTexture.lock();
		if (texture)
		{
			scaleX *= (float)texture->GetWidth();
			scaleY *= (float)texture->GetHeight();
		}
		
		const auto scale = Matrix4x4::Scale({ scaleX, scaleY, scaleZ });
		const DirectX::XMFLOAT4X4 rotation = Matrix4x4::RotationQuaternion(m_RotationQuat);
		const auto translation = Matrix4x4::Translation(position);

		m_LocalTransform = Matrix4x4::Multiply(scale, rotation);
		m_LocalTransform = Matrix4x4::Multiply(m_LocalTransform, translation);


		UpdateWorldTransform();

		
	}

}
