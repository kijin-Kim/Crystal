#pragma once
#include <DirectXMath.h>
#include "Crystal/AssetManager/ConstantBufferManager.h"

namespace Crystal {

	class Component
	{
	public:
		Component() 
		{ 
			m_ConstantBuffer = ConstantBufferManager::Instance().CreateConstantBuffer(256);
			DirectX::XMStoreFloat4x4(&m_World, DirectX::XMMatrixIdentity());
		}
		virtual ~Component()
		{
			// TODO : UNLOAD CONSTANT BUFFER
		}

		virtual void Update(float DeltaTime)
		{
			
		}

		void UpdateConstantBuffer()
		{
			m_ConstantBuffer.SetData((void*)&m_World);
		}

		void AttachToComponent(Component* parentComponent) { m_Parent = parentComponent; }
		Component* GetParent() const { return m_Parent; }

		DirectX::XMFLOAT4X4 GetTransform() { return m_World; }

	private:
		Component* m_Parent = nullptr;
		ConstantBuffer m_ConstantBuffer;
		DirectX::XMFLOAT4X4 m_World;
	};
}

