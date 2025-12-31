#include "cspch.h"
#include "Renderable.h"

namespace Crystal {


	void Renderable::Update(float deltaTime)
	{
	}

	void Renderable::Render(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> commandList, int submeshIndex, int instanceCount /*= 1*/)
	{
		D3D12_VERTEX_BUFFER_VIEW vbv[] = {
			m_VertexBuffers[submeshIndex]->AsVertexBufferView(m_VertexBuffers[submeshIndex]->GetSize() / m_VertexBuffers[submeshIndex]->GetElementCount())
		};

		commandList->IASetVertexBuffers(0, _countof(vbv), vbv);
		commandList->IASetIndexBuffer(&m_IndexBuffers[submeshIndex]->AsIndexBufferView());
		commandList->DrawIndexedInstanced(m_IndexBuffers[submeshIndex]->GetElementCount(), instanceCount, 0, 0, 0);
	}

	size_t Renderable::GetVertexbufferCount() const
	{
		return m_VertexBuffers.size();
	}


}



