#pragma once
#include <DirectXMath.h>
#include "Crystal/Resources/ConstantBuffer.h"
#include "Component.h"
#include "Crystal/Math/Math.h"

namespace Crystal {
	class TransformComponent : public Component
	{
	public:
		TransformComponent()
		{
			DirectX::XMStoreFloat4x4(&m_Transform, DirectX::XMMatrixIdentity());
		}
		virtual ~TransformComponent()
		{
		}

		virtual void Update(float deltaTime) override
		{
			Component::Update(deltaTime);
		}

		void UpdateTransform()
		{
		}

		void AttachToComponent(TransformComponent* parentComponent) { m_Parent = parentComponent; }
		TransformComponent* GetParent() const { return m_Parent; }

		const DirectX::XMFLOAT4X4& GetTransform() { return m_Transform; }

	private:
		TransformComponent* m_Parent = nullptr;
		DirectX::XMFLOAT4X4 m_Transform = Matrix4x4::Identity();
	};
}
