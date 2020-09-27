#pragma once
#include <DirectXMath.h>

namespace Crystal {

	class Component
	{
	public:
		Component() { DirectX::XMStoreFloat4x4(&m_World, DirectX::XMMatrixIdentity());}
		virtual ~Component() = default;

		virtual void Update(float DeltaTime) {}

		void AttachToComponent(Component* parentComponent) { m_Parent = parentComponent; }
		Component* GetParent() const { return m_Parent; }

		DirectX::XMFLOAT4X4 GetTransform() { return m_World; }

	private:
		Component* m_Parent = nullptr;
		DirectX::XMFLOAT4X4 m_World;
	};
}

