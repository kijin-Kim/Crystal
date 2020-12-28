#pragma once
#include <d3d12.h>
#include <wrl/client.h>


namespace Crystal {

	/* Constant Buffer View의 CPU, GPU Handle과 그 매핑된 데이터의 포인터를 가지고 있는 클래스입니다.*/
	class ConstantBuffer
	{
		friend class ConstantBufferPool;
	public:
		void SetData(void* data) { memcpy(m_CpuDataOffsetPtr, data, m_Size); }
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuDescriptorHandle() const { return m_CpuDescriptorHandle; }
		const D3D12_GPU_VIRTUAL_ADDRESS& GetGpuVirtualAddress() const { return m_GpuVirtualAddress; }

	private:
		SIZE_T m_Size = 0;
		void* m_CpuDataOffsetPtr = nullptr; // 현재 소유하고 있는 Constant Buffer View가 매핑하고 있는 데이터의 포인터입니다.
		D3D12_CPU_DESCRIPTOR_HANDLE m_CpuDescriptorHandle = {}; // 현재 소유하고 있는 Constant Buffer View의 Constant Buffer Pool상의  Cpu Handle 입니다.
		D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress = {}; // 현재 소유하고 있는 Constant Buffer View의 Constant Buffer Pool상의 GPU Handle 입니다.
	};


	/* 특정 사이즈 ( 256 / 512 / 1024 Bytes)의 Constant Buffer View를 Descriptor Heap에 미리 만들어 놓고 
	제공하는 클래스입니다. */
	class ConstantBufferPool final 
	{
		friend class ConstantBufferPoolManager;
	private:
		ConstantBufferPool() = default;
		ConstantBufferPool(int alignmentSize, int maxCbvCount);
		~ConstantBufferPool() = default;

		ConstantBuffer CreateConstantBuffer();

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CbvHeap = nullptr; // Constant Buffer View를 미리 생성해두는 CBV_SRV_UAV 타입의 힙입니다.
		D3D12_CPU_DESCRIPTOR_HANDLE m_CbvHeapCpuHandle = {}; // CBV_SRV_UAV타입의 힙 내에서 Constant Buffer View의 CPU Handle입니다.
		
		Microsoft::WRL::ComPtr<ID3D12Resource> m_ConstantBufferData = nullptr; // Constant Buffer View에 매핑되는 데이터의 풀입니다.
		UINT8* m_CpuBasePtr = nullptr; // 매핑된 데이터를 가르키는 포인터입니다.
		UINT m_BufferAlignedSize = 256; // 데이터의 정렬된 사이즈 입니다 ( 256 / 512 / 1024 Bytes)
		UINT m_CpuOffsetEnd = 0; // 빈 (마지막) 매핑된 데이터를 가르키는 포인터까지의 오프셋 입니다. (예를 들어, (Resource의 CPU 시작 포인터) + m_CpuOffset은 마지막 빈 공간을 가르킵니다)
		UINT m_MaxCbvCount = 1000; // 최대 Constant Buffer View의 갯수 입니다.
	
	};

	/* 256 / 512/ 1024 Bytes 정렬로 만들어진 Constant Buffer Pool를 관리하는 클래스입니다. Constant Buffer View를 얻으려면 
	이 클래스의 API를 이용하면 됩니다.*/
	class ConstantBufferPoolManager final
	{
	public:
		static ConstantBufferPoolManager& Instance() { static ConstantBufferPoolManager instance; return instance; }

		ConstantBuffer GetConstantBuffer(int size);

	private:
		ConstantBufferPoolManager();
		~ConstantBufferPoolManager() = default;

	private:
		ConstantBufferPool m_256AlignedBufferPool; // 256 Bytes로 정렬된 ConstantBufferPool
		ConstantBufferPool m_512AlignedBufferPool; // 512 Bytes로 정렬된 ConstantBufferPool
		ConstantBufferPool m_1024AlignedBufferPool; // 1024 Bytes로 정렬된 ConstantBufferPool
	};
}