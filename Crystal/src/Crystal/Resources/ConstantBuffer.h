#pragma once
#include "DescriptorOld.h"

namespace Crystal {
	/*µ¿Àû Constant Buffer*/
	class ConstantBuffer final : public DescriptorObjectOld
	{
	public:
		ConstantBuffer(int size);
		~ConstantBuffer() {}

		void SetData(void* data) { memcpy(m_CpuBasePtr, data, m_Size); }
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle() const { return m_ConstantBufferView.GetDescriptorHandle(); }
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return m_Resource->GetGPUVirtualAddress(); }
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_Resource = nullptr;
		DescriptorOld m_ConstantBufferView = {};
		UINT8* m_CpuBasePtr = nullptr;
		unsigned int m_Size;
	};
}
