#pragma once
#include <d3d12.h>
#include <wrl\client.h>

namespace Crystal {

	class Descriptor
	{
	public:
		Descriptor() { m_Handle.ptr = 0; }
		Descriptor(D3D12_CPU_DESCRIPTOR_HANDLE handle) : m_Handle(handle) {}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const { return m_Handle; }

		bool IsNull() const { return m_Handle.ptr == 0; }

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE m_Handle;

	};

	/*������ �۷ι� ��*/
	class DescriptorHeapManager final
	{
		friend class DescriptorObject;
	public:
		Descriptor AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType, UINT descriptorCount);
	private:
		DescriptorHeapManager();
		~DescriptorHeapManager() = default;
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CBVSRVUAVHeap = nullptr;
		UINT m_CBVSRVUAVHeapIncrementSize = 0;
		UINT m_CBVSRVUAVHeapDescriptorCount = 0;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap = nullptr;
		UINT m_RTVHeapIncrementSize = 0;
		UINT m_RTVHeapDescriptorCount = 0;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap = nullptr;
		UINT m_DSVHeapIncrementSize = 0;
		UINT m_DSVHeapDescriptorCount = 0;
	};

	/*Descriptor �� ����ϴ� Ŭ�������� ���� Ŭ�����Դϴ�
	Ŭ�����鿡�� DescriptorManager�� �����ϱ� ���� ���� Ŭ�����Դϴ�.*/
	class DescriptorObject
	{
	public:
		DescriptorObject() { if (!s_DescriptorHeapManager) s_DescriptorHeapManager = new DescriptorHeapManager(); }
		virtual ~DescriptorObject() = default;

	protected:
		Descriptor AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType, UINT descriptorCount)
		{
			return s_DescriptorHeapManager->AllocateDescriptor(descriptorHeapType, descriptorCount);
		}
	private:
		static DescriptorHeapManager* s_DescriptorHeapManager;
	};

}
