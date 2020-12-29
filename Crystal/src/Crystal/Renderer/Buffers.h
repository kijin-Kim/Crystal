#pragma once
#include <wrl/client.h>
#include "Resource.h"

namespace Crystal {
	class VertexBuffer final
	{
	public:
		VertexBuffer(void* data, UINT strideInByte, UINT count);
		VertexBuffer(const VertexBuffer&) = delete;
		VertexBuffer& operator=(const VertexBuffer&) = delete;

		const D3D12_VERTEX_BUFFER_VIEW& GetView() const { return m_VertxBufferView; }

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_DestinationBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_IntermediateBuffer = nullptr;
		D3D12_VERTEX_BUFFER_VIEW m_VertxBufferView = {};
	};

	class IndexBuffer final
	{
	public:
		IndexBuffer(void* data, UINT sizeInByte, UINT count);
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;

		const D3D12_INDEX_BUFFER_VIEW& GetView() const { return m_IndexBufferView; }

		UINT GetCount() const { return m_Count; }

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_DestinationBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_IntermediateBuffer = nullptr;

		D3D12_INDEX_BUFFER_VIEW m_IndexBufferView = {};
		UINT m_Count = 0;
	};
}
