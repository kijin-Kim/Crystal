#pragma once
#include <d3d12.h>
#include <wrl/client.h>

namespace Crystal {

	/* Constant Buffer View�� CPU, GPU Handle�� �� ���ε� �������� �����͸� ������ �ִ� ������ ��� �Դϴ�.*/
	struct ConstantBufferData
	{
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

		ConstantBufferData CreateConstantBuffer();

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CbvHeap = nullptr; // Constant Buffer View�� �̸� �����صδ� CBV_SRV_UAV Ÿ���� ���Դϴ�.
		D3D12_CPU_DESCRIPTOR_HANDLE m_CbvHeapCpuHandle = {}; // CBV_SRV_UAVŸ���� �� ������ Constant Buffer View�� CPU Handle�Դϴ�.

		Microsoft::WRL::ComPtr<ID3D12Resource> m_ConstantBufferData = nullptr; // Constant Buffer View�� ���εǴ� �������� Ǯ�Դϴ�.
		UINT8* m_CpuBasePtr = nullptr; // ���ε� �����͸� ����Ű�� �������Դϴ�.
		UINT m_BufferAlignedSize = 256; // �������� ���ĵ� ������ �Դϴ� ( 256 / 512 / 1024 Bytes)
		UINT m_CpuOffsetEnd = 0; // �� (������) ���ε� �����͸� ����Ű�� �����ͱ����� ������ �Դϴ�. (���� ���, (Resource�� CPU ���� ������) + m_CpuOffset�� ������ �� ������ ����ŵ�ϴ�)
		UINT m_MaxCbvCount = 1000; // �ִ� Constant Buffer View�� ���� �Դϴ�.
	};

	/* 256 / 512/ 1024 Bytes ���ķ� ������� Constant Buffer Pool�� �����ϴ� Ŭ�����Դϴ�. */
	class ConstantBufferPoolManager final
	{
		friend class ConstantBuffer;
	private:
		ConstantBufferPoolManager();
		~ConstantBufferPoolManager() = default;

		ConstantBufferData CreateConstantBuffer(int size);

	private:
		ConstantBufferPool m_256AlignedBufferPool; // 256 Bytes�� ���ĵ� ConstantBufferPool
		ConstantBufferPool m_512AlignedBufferPool; // 512 Bytes�� ���ĵ� ConstantBufferPool
		ConstantBufferPool m_1024AlignedBufferPool; // 1024 Bytes�� ���ĵ� ConstantBufferPool
	};

	class ConstantBuffer final
	{
	public:
		ConstantBuffer(int size) 
		{ 
			if (!s_ConstantBufferPoolManager)
				s_ConstantBufferPoolManager = new ConstantBufferPoolManager();
				m_ConstantBufferData = s_ConstantBufferPoolManager->CreateConstantBuffer(size); 
		}
		~ConstantBuffer() { /*s_ConstantBufferPoolManager.ReleaseConstantBuffer(m_ConstantBufferData); */ }

		void SetData(void* data) { memcpy(m_ConstantBufferData.m_CpuDataOffsetPtr, data, m_ConstantBufferData.m_Size); }
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle() const { return m_ConstantBufferData.m_CpuDescriptorHandle; }
		D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return m_ConstantBufferData.m_GpuVirtualAddress; }

	private:
		ConstantBufferData m_ConstantBufferData;
		static ConstantBufferPoolManager* s_ConstantBufferPoolManager;
	};
}