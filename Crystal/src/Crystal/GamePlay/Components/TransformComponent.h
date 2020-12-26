#pragma once
#include <DirectXMath.h>
#include "Crystal/AssetManager/ConstantBufferManager.h"
#include "Component.h"

namespace Crystal {

	class TransformComponent : public Component
	{
	public:
		TransformComponent() 
		{ 
			m_ConstantBuffer = ConstantBufferManager::Instance().CreateConstantBuffer(256);
			DirectX::XMStoreFloat4x4(&m_World, DirectX::XMMatrixIdentity());
		}
		virtual ~TransformComponent()
		{
			// TODO : UNLOAD CONSTANT BUFFER
		}

		virtual void Update(float deltaTime) override
		{
			Component::Update(deltaTime);
		}

		void UpdateConstantBuffer()
		{
			m_ConstantBuffer.SetData((void*)&m_World);
		}

		void AttachToComponent(TransformComponent* parentComponent) { m_Parent = parentComponent; }
		TransformComponent* GetParent() const { return m_Parent; }

		DirectX::XMFLOAT4X4 GetTransform() { return m_World; }

	private:
		TransformComponent* m_Parent = nullptr;
		ConstantBuffer m_ConstantBuffer;
		DirectX::XMFLOAT4X4 m_World;
	};
}

