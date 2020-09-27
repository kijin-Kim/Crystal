#pragma once
#include <d3d12.h>
#include <wrl/client.h>


namespace Crystal {

	class ConstantBuffer
	{
		friend class ConstantBufferManager;
	public:
		void SetData(void* data) { memcpy(m_CpuDataOffsetPtr, data, m_Size); }
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetView() const { return m_CpuDescriptorHandle; }

	private:
		SIZE_T m_Size = 256;
		UINT8* m_CpuDataOffsetPtr = 0;
		D3D12_CPU_DESCRIPTOR_HANDLE m_CpuDescriptorHandle = {};
	};


	// max 256 From Now
	class ConstantBufferManager final 
	{
	public:
		static ConstantBufferManager& Get() { static ConstantBufferManager instance; return instance; }
		ConstantBuffer CreateConstantBuffer(UINT dataSize = 256);

	private:
		ConstantBufferManager();
		~ConstantBufferManager() = default;

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CbvPool = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE m_CbvPoolCpuHandle = {};
		
		Microsoft::WRL::ComPtr<ID3D12Resource> m_ConstantBufferDataPool = nullptr; // 256 Byte
		UINT8* m_CpuBasePtr = 0;
		UINT m_BufferSize = 256;
		UINT m_CpuOffsetEnd = 0; // 256 Byte

		
		UINT m_CbvCount = 1000;
		

		
	};

}