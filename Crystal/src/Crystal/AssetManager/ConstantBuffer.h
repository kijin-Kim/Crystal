#pragma once
#include <d3d12.h>
#include <wrl/client.h>


namespace Crystal {

	/* Constant Buffer View�� CPU, GPU Handle�� �� ���ε� �������� �����͸� ������ �ִ� Ŭ�����Դϴ�.*/
	class ConstantBuffer
	{
		friend class ConstantBufferPool;
	public:
		void SetData(void* data) { memcpy(m_CpuDataOffsetPtr, data, m_Size); }
		const D3D12_CPU_DESCRIPTOR_HANDLE& GetCpuDescriptorHandle() const { return m_CpuDescriptorHandle; }
		const D3D12_GPU_VIRTUAL_ADDRESS& GetGpuVirtualAddress() const { return m_GpuVirtualAddress; }

	private:
		SIZE_T m_Size = 0;
		void* m_CpuDataOffsetPtr = nullptr; // ���� �����ϰ� �ִ� Constant Buffer View�� �����ϰ� �ִ� �������� �������Դϴ�.
		D3D12_CPU_DESCRIPTOR_HANDLE m_CpuDescriptorHandle = {}; // ���� �����ϰ� �ִ� Constant Buffer View�� Constant Buffer Pool����  Cpu Handle �Դϴ�.
		D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress = {}; // ���� �����ϰ� �ִ� Constant Buffer View�� Constant Buffer Pool���� GPU Handle �Դϴ�.
	};


	/* Ư�� ������ ( 256 / 512 / 1024 Bytes)�� Constant Buffer View�� Descriptor Heap�� �̸� ����� ���� 
	�����ϴ� Ŭ�����Դϴ�. */
	class ConstantBufferPool final 
	{
		friend class ConstantBufferPoolManager;
	private:
		ConstantBufferPool() = default;
		ConstantBufferPool(int alignmentSize, int maxCbvCount);
		~ConstantBufferPool() = default;

		ConstantBuffer CreateConstantBuffer();

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CbvHeap = nullptr; // Constant Buffer View�� �̸� �����صδ� CBV_SRV_UAV Ÿ���� ���Դϴ�.
		D3D12_CPU_DESCRIPTOR_HANDLE m_CbvHeapCpuHandle = {}; // CBV_SRV_UAVŸ���� �� ������ Constant Buffer View�� CPU Handle�Դϴ�.
		
		Microsoft::WRL::ComPtr<ID3D12Resource> m_ConstantBufferData = nullptr; // Constant Buffer View�� ���εǴ� �������� Ǯ�Դϴ�.
		UINT8* m_CpuBasePtr = nullptr; // ���ε� �����͸� ����Ű�� �������Դϴ�.
		UINT m_BufferAlignedSize = 256; // �������� ���ĵ� ������ �Դϴ� ( 256 / 512 / 1024 Bytes)
		UINT m_CpuOffsetEnd = 0; // �� (������) ���ε� �����͸� ����Ű�� �����ͱ����� ������ �Դϴ�. (���� ���, (Resource�� CPU ���� ������) + m_CpuOffset�� ������ �� ������ ����ŵ�ϴ�)
		UINT m_MaxCbvCount = 1000; // �ִ� Constant Buffer View�� ���� �Դϴ�.
	
	};

	/* 256 / 512/ 1024 Bytes ���ķ� ������� Constant Buffer Pool�� �����ϴ� Ŭ�����Դϴ�. Constant Buffer View�� �������� 
	�� Ŭ������ API�� �̿��ϸ� �˴ϴ�.*/
	class ConstantBufferPoolManager final
	{
	public:
		static ConstantBufferPoolManager& Instance() { static ConstantBufferPoolManager instance; return instance; }

		ConstantBuffer GetConstantBuffer(int size);

	private:
		ConstantBufferPoolManager();
		~ConstantBufferPoolManager() = default;

	private:
		ConstantBufferPool m_256AlignedBufferPool; // 256 Bytes�� ���ĵ� ConstantBufferPool
		ConstantBufferPool m_512AlignedBufferPool; // 512 Bytes�� ���ĵ� ConstantBufferPool
		ConstantBufferPool m_1024AlignedBufferPool; // 1024 Bytes�� ���ĵ� ConstantBufferPool
	};
}