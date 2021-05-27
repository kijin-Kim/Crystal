#pragma once
#include "DescriptorAllocation.h"

namespace Crystal {

	
	class Buffer
	{
	public:
		Buffer(void* data, UINT sizeInByte, UINT count, bool bAlign256, bool bAsDynamic);
		~Buffer() {}

		void SetData(void* data, uint32_t offset, uint32_t size);
		

		D3D12_CPU_DESCRIPTOR_HANDLE GetConstantBufferView() const;
		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

		void CreateConstantBufferView();


		uint32_t GetElementCount() const;
		uint32_t GetSize() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		DescriptorAllocation m_ConstantBufferView = {};
		UINT8* m_CpuBasePtr = nullptr;

		uint32_t m_Size;
		uint32_t m_ElementCount;
	};
}
