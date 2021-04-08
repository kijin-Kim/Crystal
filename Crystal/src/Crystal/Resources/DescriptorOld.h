#pragma once
#include <d3d12.h>
#include <wrl/client.h>

namespace Crystal {
	class DescriptorOld
	{
	public:
		DescriptorOld() { m_Handle.ptr = 0; }
		DescriptorOld(D3D12_CPU_DESCRIPTOR_HANDLE handle) : m_Handle(handle) {}

		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle() const { return m_Handle; }

		bool IsNull() const { return m_Handle.ptr == 0; }

	public:
		D3D12_CPU_DESCRIPTOR_HANDLE m_Handle;
	};

	/*간단한 글로벌 힙*/
	class DescriptorHeapManagerOld final
	{
		friend class DescriptorObjectOld;
	public:
		DescriptorOld AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType);
	private:
		DescriptorHeapManagerOld();
		~DescriptorHeapManagerOld() = default;
	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CBVSRVUAVHeap = nullptr;
		UINT m_CBVSRVUAVHeapIncrementSize = 0;
		UINT m_CBVSRVUAVHeapDescriptorCount = 0;
		const UINT m_MaxCBVSRVUAVHeapDescriptorCount = 3000;
		
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap = nullptr;
		UINT m_RTVHeapIncrementSize = 0;
		UINT m_RTVHeapDescriptorCount = 0;
		const UINT m_MaxRTVHeapDescriptorCount = 30;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap = nullptr;
		UINT m_DSVHeapIncrementSize = 0;
		UINT m_DSVHeapDescriptorCount = 0;
		const UINT m_MaxDSVHeapDescriptorCount = 30;
	};

	/*Descriptor 를 사용하는 클래스들의 상위 클래스입니다
	클래스들에게 DescriptorManager를 제공하기 위한 상위 클래스입니다.*/
	class DescriptorObjectOld
	{
	public:
		DescriptorObjectOld() { if (!s_DescriptorHeapManager) s_DescriptorHeapManager = new DescriptorHeapManagerOld(); }
		virtual ~DescriptorObjectOld() = default;

	protected:
		DescriptorOld AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE descriptorHeapType)
		{
			return s_DescriptorHeapManager->AllocateDescriptor(descriptorHeapType);
		}
	private:
		static DescriptorHeapManagerOld* s_DescriptorHeapManager;
	};
}
