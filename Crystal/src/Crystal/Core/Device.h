#pragma once
#include <d3d12.h>

#include "Crystal/Resources/DescriptorAllocator.h"


namespace Crystal {
	class DescriptorAllocator;
	class CommandQueue;

	class Device
	{
	public:
		static Device& Instance()
		{
			static Device instance;
			return instance;
		}

		Device(const Device& other) = delete;
		Device(Device&& other) noexcept = delete;
		Device& operator=(const Device& other) = delete;
		Device& operator=(Device&& other) noexcept = delete;

		ID3D12Device2* GetD3DDevice() const { return m_Device.Get(); }
		IDXGIFactory4* GetFactory() const { return m_Factory.Get(); }

		CommandQueue* GetCommandQueue() const;


		DescriptorAllocation AllocateDescriptors(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t descriptorCount);
		void ReleaseStaleDescriptors();

		UINT GetIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE type);


	private:
		Device();
		~Device() = default;

	private:
		Microsoft::WRL::ComPtr<ID3D12Device2> m_Device;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_Fence = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory = nullptr;

		std::unique_ptr<CommandQueue> m_CommandQueue = nullptr;


		std::unique_ptr<DescriptorAllocator> m_DescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

		HANDLE m_FenceEvent;


		UINT m_IncrementSizes[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];
		
	};
}


