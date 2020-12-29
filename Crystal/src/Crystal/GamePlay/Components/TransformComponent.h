#pragma once
#include <DirectXMath.h>
#include "Crystal/AssetManager/ConstantBuffer.h"
#include "Component.h"

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

		DirectX::XMFLOAT4X4 GetTransform() { return m_Transform; }

	private:
		TransformComponent* m_Parent = nullptr;
		DirectX::XMFLOAT4X4 m_Transform;
	};
}
