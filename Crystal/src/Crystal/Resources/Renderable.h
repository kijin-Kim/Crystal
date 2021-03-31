#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include "Crystal/Resources/Buffers.h"

namespace Crystal {
	class Renderable
	{
	public:
		Renderable() = default;
		virtual ~Renderable() = default;

		virtual void Update(float deltaTime) {}
		virtual void Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, int submeshIndex)
		{
			commandList->IASetVertexBuffers(0, 1, &m_VertexBuffers[submeshIndex]->GetView());
			commandList->IASetIndexBuffer(&m_IndexBuffers[submeshIndex]->GetView());
			commandList->DrawIndexedInstanced(m_IndexBuffers[submeshIndex]->GetCount(), 1, 0, 0, 0);
		}

		size_t GetVertexbufferCount() const { return m_VertexBuffers.size(); }

	protected:
		std::vector <std::unique_ptr<VertexBuffer>> m_VertexBuffers;
		std::vector <std::unique_ptr<IndexBuffer>> m_IndexBuffers;
	};
}